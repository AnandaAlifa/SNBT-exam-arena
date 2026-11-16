#include "leaderboard.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define LEADERBOARD_FILE "leaderboard.dat"
#define LEADERBOARD_MAGIC 0x53424E54u
#define LEADERBOARD_VERSION 1

typedef struct {
    unsigned int magic;
    int version;
    int count;
} LeaderboardHeader;

static void CopyEntryName(char *destination, size_t size, const char *source)
{
    if (size == 0) {
        return;
    }

    if (source == NULL) {
        source = "";
    }

    while (*source != '\0' && isspace((unsigned char)*source)) {
        ++source;
    }

    size_t length = strlen(source);
    while (length > 0 && isspace((unsigned char)source[length - 1])) {
        --length;
    }
    if (length >= size) {
        length = size - 1;
    }

    memmove(destination, source, length);
    destination[length] = '\0';

    if (destination[0] == '\0') {
        snprintf(destination, size, "Peserta");
    }
}

static void NormalizeEntry(LeaderboardEntry *entry)
{
    entry->name[sizeof(entry->name) - 1] = '\0';
    CopyEntryName(entry->name, sizeof(entry->name), entry->name);

    if (entry->score < 0) {
        entry->score = 0;
    }
    if (entry->difficultyUsed < 0 || entry->difficultyUsed >= DIFFICULTY_COUNT) {
        entry->difficultyUsed = DIFFICULTY_HARD;
    }
    if (entry->subtest < 0 || entry->subtest >= SUBTEST_COUNT) {
        entry->subtest = 0;
    }
}

static void SortEntries(LeaderboardEntry *entries, int count)
{
    for (int i = 1; i < count; ++i) {
        LeaderboardEntry key = entries[i];
        int j = i - 1;

        while (j >= 0) {
            bool lowerScore = entries[j].score < key.score;
            bool sameScoreLaterName = entries[j].score == key.score &&
                                      strcmp(entries[j].name, key.name) > 0;
            if (!lowerScore && !sameScoreLaterName) {
                break;
            }
            entries[j + 1] = entries[j];
            --j;
        }
        entries[j + 1] = key;
    }
}

static bool SaveEntries(const LeaderboardEntry *entries, int count)
{
    FILE *file = fopen(LEADERBOARD_FILE, "wb");
    if (file == NULL) {
        return false;
    }

    LeaderboardHeader header = {
        LEADERBOARD_MAGIC,
        LEADERBOARD_VERSION,
        count
    };

    bool ok = fwrite(&header, sizeof(header), 1, file) == 1;
    if (ok && count > 0) {
        ok = fwrite(entries, sizeof(LeaderboardEntry), (size_t)count, file) ==
             (size_t)count;
    }
    if (fclose(file) != 0) {
        ok = false;
    }
    return ok;
}

void LeaderboardLoad(Game *game)
{
    game->leaderboardCount = 0;

    FILE *file = fopen(LEADERBOARD_FILE, "rb");
    if (file == NULL) {
        return;
    }

    LeaderboardHeader header;
    if (fread(&header, sizeof(header), 1, file) != 1 ||
        header.magic != LEADERBOARD_MAGIC ||
        header.version != LEADERBOARD_VERSION ||
        header.count < 0 || header.count > LEADERBOARD_MAX) {
        fclose(file);
        return;
    }

    LeaderboardEntry entries[LEADERBOARD_MAX];
    int count = header.count;
    if (count > 0 &&
        fread(entries, sizeof(LeaderboardEntry), (size_t)count, file) !=
            (size_t)count) {
        fclose(file);
        return;
    }
    fclose(file);

    for (int i = 0; i < count; ++i) {
        NormalizeEntry(&entries[i]);
    }
    SortEntries(entries, count);

    memcpy(game->leaderboard, entries,
           sizeof(LeaderboardEntry) * (size_t)count);
    game->leaderboardCount = count;
}

bool LeaderboardSubmit(Game *game)
{
    if (game->session.questionCount <= 0) {
        return false;
    }

    LeaderboardEntry combined[LEADERBOARD_MAX + 1];
    int count = game->leaderboardCount;
    if (count < 0) {
        count = 0;
    } else if (count > LEADERBOARD_MAX) {
        count = LEADERBOARD_MAX;
    }

    if (count > 0) {
        memcpy(combined, game->leaderboard,
               sizeof(LeaderboardEntry) * (size_t)count);
    }

    LeaderboardEntry entry;
    memset(&entry, 0, sizeof(entry));
    CopyEntryName(entry.name, sizeof(entry.name), game->session.playerName);
    entry.score = game->session.finalScore;
    entry.difficultyUsed = game->session.currentDifficulty;
    entry.subtest = game->session.currentSubtest;
    combined[count++] = entry;

    SortEntries(combined, count);
    if (count > LEADERBOARD_MAX) {
        count = LEADERBOARD_MAX;
    }

    memcpy(game->leaderboard, combined,
           sizeof(LeaderboardEntry) * (size_t)count);
    game->leaderboardCount = count;
    return SaveEntries(game->leaderboard, count);
}

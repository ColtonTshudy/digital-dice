#include "stm32f4xx_hal.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Statistics {
    uint16_t *roll_counts;
    float *expect;
    uint8_t sides;
    uint8_t number_of_dice;
    uint8_t min_roll;
    uint8_t max_roll;
    uint16_t number_of_rolls;
    uint16_t *current_rolls;
};

void initStats(struct Statistics *stats, uint8_t sides, uint8_t number_of_dice);
float *generateDiceProbabilities(uint8_t s, uint8_t n, uint8_t max_roll, uint8_t min_roll);
uint64_t combinations(uint8_t n, uint8_t k);
float diceRollProbability(uint8_t p, uint8_t n, uint8_t s);
void printStats(struct Statistics *stats);
void roll(struct Statistics *stats);
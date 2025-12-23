#include "app_stats.h"

extern ADC_HandleTypeDef hadc1;
bool isADCFinished = false;

/**
 * @brief 
 * 
 * @param sides - number of sides on the dice
 * @param numberOfDice - how many dice to roll
 * @return struct Statistics* 
 */
void initStats(struct Statistics *stats, uint8_t sides, uint8_t number_of_dice) {
    stats->sides = sides;
    stats->number_of_dice = number_of_dice;

    uint16_t min_roll = number_of_dice;
    uint16_t max_roll = sides * number_of_dice;

    stats->min_roll = min_roll;
    stats->max_roll = max_roll;
    stats->roll_counts = 0;

    stats->expect = generateDiceProbabilities(sides, number_of_dice, max_roll, min_roll);

    stats->roll_counts = malloc(sizeof(uint16_t) * (max_roll + 1));
    memset(stats->roll_counts, 0, sizeof(uint16_t) * (max_roll + 1));

    stats->current_rolls = malloc(sizeof(uint16_t) * (number_of_dice));
    memset(stats->current_rolls, 0, sizeof(uint16_t) * (number_of_dice));

    printStats(stats);
}

void roll(struct Statistics *stats) {
    uint16_t roll_total = 0;
    for (uint8_t i = 0; i < stats->number_of_dice; i++) {
        uint16_t roll = 1 + rand() / (RAND_MAX / (stats->sides));
        stats->current_rolls[i] = roll;
        printf("Roll %u: %u\r\n", i, roll);
        roll_total += roll;
    }
    stats->roll_counts[roll_total]++;
    stats->number_of_rolls++;
}

void printStats(struct Statistics *stats) {
    printf("Statistics Debug:\r\n");
    printf("  sides: %u\r\n", stats->sides);
    printf("  number_of_dice: %u\r\n", stats->number_of_dice);
    printf("  min_roll: %u\r\n", stats->min_roll);
    printf("  max_roll: %u\r\n", stats->max_roll);
    for (uint8_t i = 0; i < stats->max_roll + 1; i++) {
        printf("  Probability of rolling [%u]: %.6f\r\n", i, stats->expect[i]);
    }
    for (uint8_t i = 0; i < stats->max_roll + 1; i++) {
        printf("  Roll counts for [%u]: %u\r\n", i, stats->roll_counts[i]);
    }
}

/**
 * @brief Compute probabilities of rolling number "p" on "n" "s"-sided dice.
 * 
 * @param s - sides on die
 * @param n - number of dice
 * @param max_roll - maximum rollable value
 * @param min_roll - minimum rollable value
 * @return float* 
 */
float *generateDiceProbabilities(uint8_t s, uint8_t n, uint8_t max_roll, uint8_t min_roll) {
    // Initialize an array to hold the number of combinations that exist for each dice roll result
    // e.g. coeff_array[7] = 6 for 2 D6
    // Initialize to 1 for all values before we do the math (assume 1 die)
    // approx 1KB of memory for 4x d20
    // uint16_t *coeff_array = malloc(sizeof(float) * (max_roll + 1));
    // memset(&coeff_array[min_roll], 1, sizeof(float) * (max_roll - min_roll + 1));

    // Turn coefficients into probabilities
    // approx 2KB of memory for 4x d20
    float *probabilities = malloc(sizeof(float) * (max_roll + 1));
    memset(probabilities, 0, sizeof(float) * (min_roll + 1));

    for (uint16_t i = min_roll; i < max_roll + 1; i++) {
        // Find probability of rolling a <i> given we have <n> dice with <s> sides each
        float probability = diceRollProbability(i, n, s);
        probabilities[i] = probability;
        // printf("[generateDiceProbabilities] Probability of rolling [%u]: %.6f\r\n", i, probabilities[i]);
    }

    // Delete the temporary coefficient array
    // free(coeff_array);

    return probabilities;
}

/**
 * @brief calculate the probability of rolling value "p" on "n" "s"-sided dice. 
 * First calculates number of combinations to get the roll "p", then normalizes
 * over total number of possible rolls.
 * See equation (10) from https://mathworld.wolfram.com/Dice.html
 * 
 */
float diceRollProbability(uint8_t p, uint8_t n, uint8_t s) {
    uint8_t k_max = (p - n) / s;
    int c = 0;

    for (uint8_t k = 0; k <= k_max; k++) {
        uint64_t comb_1 = combinations(n, k);
        uint64_t comb_2 = combinations((uint8_t)(p - s * k - 1), n - 1);
        c += pow(-1, k) * comb_1 * comb_2;
    }

    return ((float)c) / pow(s, n);
}

/**
 * @brief Calculate combinations (n choose k) via the binomial theorem
 * 
 */
uint64_t combinations(uint8_t n, uint8_t k) {
    // Edge cases
    if (k > n)
        return 0;
    if (k == 0 || k == n)
        return 1;

    // Optimize loop count in a later step by using a smaller k value
    // since C(n,k) = C(n, n-k)
    if (k > (n - k))
        k = n - k;

    // Calculate the number of combinations
    // Warning! subject to intermediate overflow due to multiplication
    // Maybe implement some flag if overflow occurs, or use STM's built in flag
    uint64_t result = 1;
    for (uint8_t i = 0; i < k; i++) {
        result = (result * (n - i)) / (i + 1);
    }
    return result;
}

void setRandSeed(uint32_t seed) {
    srand(seed);
}

uint32_t generateRandSeed() {
    uint32_t seed = 0;
    uint16_t adc_buffer[4] = {0};
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 4);
    uint8_t offset = 0;
    while (offset < 32 / 2) {
        if (isADCFinished) {
            isADCFinished = false;
            uint8_t i = 0;
            while (i < 4 && offset < 32 / 2) {
                // printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(adc_buffer[i]));
                seed += (adc_buffer[3 - i++] & 0x0f) << (2 * offset++);
            }
            // printf("\r\n");

            if (offset < 32 / 2) {
                HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 4);
            }
        }
    }
    HAL_ADC_Stop_DMA(&hadc1);

    // uint16_t maxes[5] = {0};
    // uint16_t mins[5] ={9999,9999,9999,9999,9999};
    // HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 5);
    // uint8_t i = 0;
    // while (i<100) {
    //     if (isADCFinished) {

    //         isADCFinished = false;
    //         uint8_t j = 0;
    //         while (j < 5) {
    //             // printf("%u,\t", adc_buffer[j]);
    //             if (maxes[j] < adc_buffer[j]){
    //                 maxes[j] = adc_buffer[j];
    //             }
    //             if (mins[j] > adc_buffer[j]){
    //                 mins[j] = adc_buffer[j];
    //             }
    //             j++;
    //         }
    //         // printf("\r\n");
    //         if(i<99){
    //             HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 5);
    //         }
    //         i++;
    //     }
    // }
    // HAL_ADC_Stop_DMA(&hadc1);

    // printf("===============================RANGE     ");
    // uint8_t j = 0;
    // while (j < 5) {
    //     printf("%u,\t", maxes[j]-mins[j++]);
    // }
    // printf("\r\n");

    // printf("===============================MAXES     ");
    // j = 0;
    // while (j < 5) {
    //     printf("%u,\t", maxes[j++]);
    // }
    // printf("\r\n");

    // printf("===============================MINS     ");
    // j = 0;
    // while (j < 5) {
    //     printf("%u,\t", mins[j++]);
    // }
    // printf("\r\n");

    // printf("===============================BIT SWEEP     ");
    // j = 0;
    // while (j < 5) {
    //     printf("%f,\t", log2(maxes[j]-mins[j++]));
    // }
    // printf("\r\n");

    printf("Seed generated: %lu\r\n", seed);
    return seed;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    (void)hadc;
    isADCFinished = true;
}
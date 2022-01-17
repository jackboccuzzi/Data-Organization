/**
 * climate.c
 *
 * Performs analysis on climate data provided by the
 * National Oceanic and Atmospheric Administration (NOAA).
 *
 * Input:    Tab-delimited file(s) to analyze.
 * Output:   Summary information about the data.
 *
 * Compile:  run make
 *
 * Example Run:      ./climate data_tn.tdv data_wa.tdv
 *
 *
 * Opening file: data_tn.tdv
 * Opening file: data_wa.tdv
 * States found: TN WA
 * -- State: TN --
 * Number of Records: 17097
 * Average Humidity: 49.4%
 * Average Temperature: 58.3F
 * Max Temperature: 110.4F 
 * Max Temperatuer on: Mon Aug  3 11:00:00 2015
 * Min Temperature: -11.1F
 * Min Temperature on: Fri Feb 20 04:00:00 2015
 * Lightning Strikes: 781
 * Records with Snow Cover: 107
 * Average Cloud Cover: 53.0%
 * -- State: WA --
 * Number of Records: 48357
 * Average Humidity: 61.3%
 * Average Temperature: 52.9F
 * Max Temperature: 125.7F
 * Max Temperature on: Sun Jun 28 17:00:00 2015
 * Min Temperature: -18.7F 
 * Min Temperature on: Wed Dec 30 04:00:00 2015
 * Lightning Strikes: 1190
 * Records with Snow Cover: 1383
 * Average Cloud Cover: 54.5%
 *
 * TDV format:
 * 
 * CA» 1428300000000»  9prcjqk3yc80»   93.0»   0.0»100.0»  0.0»95644.0»277.58716
 * CA» 1430308800000»  9prc9sgwvw80»   4.0»    0.0»100.0»  0.0»99226.0»282.63037
 * CA» 1428559200000»  9prrremmdqxb»   61.0»   0.0»0.0»0.0»102112.0»   285.07513
 * CA» 1428192000000»  9prkzkcdypgz»   57.0»   0.0»100.0»  0.0»101765.0» 285.21332
 * CA» 1428170400000»  9prdd41tbzeb»   73.0»   0.0»22.0»   0.0»102074.0» 285.10425
 * CA» 1429768800000»  9pr60tz83r2p»   38.0»   0.0»0.0»0.0»101679.0»   283.9342
 * CA» 1428127200000»  9prj93myxe80»   98.0»   0.0»100.0»  0.0»102343.0» 285.75
 * CA» 1428408000000»  9pr49b49zs7z»   93.0»   0.0»100.0»  0.0»100645.0» 285.82413
 *
 * Each field is separated by a tab character \t and ends with a newline \n.
 *
 * Fields:
 *      state code (e.g., CA, TX, etc),
 *      timestamp (time of observation as a UNIX timestamp),
 *      geolocation (geohash string),
 *      humidity (0 - 100%),
 *      snow (1 = snow present, 0 = no snow),
 *      cloud cover (0 - 100%),
 *      lightning strikes (1 = lightning strike, 0 = no lightning),
 *      pressure (Pa),
 *      surface temperature (Kelvin)
 */

#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_STATES 50

/* TODO: Add elements to the climate_info struct as necessary. */
struct climate_info {
    char code[3];
    unsigned long num_records;
    long double sum_temperature;
    long double sum_humidity;
    double max_temperature;
    long max_temp_date;
    double min_temperature;
    long min_temp_date;
    unsigned long num_lightning;
    unsigned long num_snowcover;
    long double sum_cloudcover;
};

void analyze_file(FILE *file, struct climate_info *states[], int num_states);
void print_report(struct climate_info *states[], int num_states);

int findStateIndex(struct climate_info **states, char *stateCode){
    // loop until we hit a NULL spot.
    int i;
    for(i = 0; states[i] != NULL; i++) {
        if(strcmp(states[i]->code, stateCode) == 0) {
            return i;
        }
    }
    // 0 can't be negative, need to return a negative number to create a new state struct.
    if (i == 0) {
        return -50;
    }
    else {
        return -i;
    }
}

int main(int argc, char *argv[]) {

    /* TODO: fix this conditional. You should be able to read multiple files. */
    if (argc < 2) { 
        printf("Usage: %s tdv_file1 tdv_file2 ... tdv_fileN \n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Let's create an array to store our state data in. As we know, there are
     * 50 US states. */
    struct climate_info *states[NUM_STATES] = {NULL};

    int i;
    for (i = 1; i < argc; ++i) {
        /* TODO: Open the file for reading */
        FILE *file = fopen(argv[i], "r");

        /* TODO: If the file doesn't exist, print an error message and move on
         * to the next file. */
        if (file == NULL) {
            printf("Error: File \"%s\" does not exist.\n", argv[i]);
            continue;
        }

        /* TODO: Analyze the file */
        analyze_file(file, states, NUM_STATES);
        fclose(file);
    }

    /* Now that we have recorded data for each file, we'll summarize them: */
    print_report(states, NUM_STATES);

    return 0;
}

// (file pointer, array of climate_info structs, number of states)
void analyze_file(FILE *file, struct climate_info **states/* *states[]*/, int num_states) {
    const int line_sz = 100;
    char line[line_sz];
    char *token;
    char delim[2] = {'\t'};
    struct climate_info *new_state;

    while (fgets(line, line_sz, file) != NULL) {

        /* TODO: We need to do a few things here:
         *
         *       * Tokenize the line.
         *       * Determine what state the line is for. This will be the state
         *         code, stored as our first token.
         *       * If our states array doesn't have a climate_info entry for
         *         this state, then we need to allocate memory for it and put it
         *         in the next open place in the array. Otherwise, we reuse the
         *         existing entry.
         *       * Update the climate_info structure as necessary.
         */

        // ----------------------STATE CODE TOKEN--------------------
        token = strtok(line, delim);
        char* state_code = token;

        // determine if state already exists in array
        int state_index = findStateIndex(states, state_code);

        // if state does not exist, create new entry
        if (state_index < 0) {
            // allocate memory for new state
            new_state = (struct climate_info*) malloc(sizeof(struct climate_info));
            // copy state code
            strcpy(new_state->code, state_code);

            // initialize new state
            new_state->num_records = 0;
            new_state->sum_temperature = 0;
            new_state->sum_humidity = 0;
            new_state->max_temperature = -1000;
            new_state->min_temperature = 1000;
            new_state->num_lightning = 0;
            new_state->num_snowcover = 0;
            new_state->sum_cloudcover = 0;
            
            // get the index of the next open spot in the array
            if (state_index == -50) {
                state_index = 0;
            }
            else {
                state_index = state_index * -1;
            }

            // add new state to array
            states[state_index] = new_state;
        }
        // increment number of records (could be at the end????)
        states[state_index]->num_records++;
        // ----------------------------------------------------------

        // ----------------------TIMESTAMP TOKEN---------------------
        token = strtok(NULL, delim);
        char* timestamp = token;
        long timestamp_long = atol(timestamp) / 1000;
        // ----------------------------------------------------------

        // ------------------GEOLOCATION TOKEN-----------------------
        token = strtok(NULL, delim);
        // char* geolocation = token;
        // ----------------------------------------------------------

        // ---------------------HUMIDITY TOKEN-----------------------
        token = strtok(NULL, delim); // 0 -- 100%
        char* humidity = token;
        // convert the string to a long double
        long double humidity_val = atof(humidity);
        // add to the total humidity to calculate average later
        new_state->sum_humidity += humidity_val; 
        // ----------------------------------------------------------

        // ---------------------SNOW TOKEN---------------------------
        token = strtok(NULL, delim); // 0.0 or 1.0
        char* snow = token;
        // convert the string to a long double
        long double snow_val = atof(snow);
        // add to the total amounts snow cover
        new_state->num_snowcover += snow_val;
        // ----------------------------------------------------------

        // -------------------CLOUD COVERAGE TOKEN-------------------
        token = strtok(NULL, delim); // 0 -- 100%
        char* cloudcover = token;
        // convert the string to a long double
        long double cloudcover_val = atof(cloudcover);
        // add to the total cloud cover to calculate average later
        new_state->sum_cloudcover += cloudcover_val;
        // ----------------------------------------------------------

        // ---------------------LIGHTNING TOKEN----------------------
        token = strtok(NULL, delim); // 0.0 or 1.0
        char* lightning = token;
        // convert the string to a long double
        long double lightning_val = atof(lightning);
        // add to the total number of lightning strikes
        new_state->num_lightning +=  lightning_val;
        // ----------------------------------------------------------

        // ---------------------PRESSURE TOKEN-----------------------
        token = strtok(NULL, delim); 
        // char* pressure = token;
        // ----------------------------------------------------------

        // ----------------SURFACE TEMPERATURE TOKEN-----------------
        token = strtok(NULL, delim);
        char* temperature = token;
        // convert string to long double
        long double temperature_val = atof(temperature);
        // convert the temp in 'K' to 'F'
        temperature_val = (temperature_val * 1.8) - 459.67;
        
        // add temperature to sum to calculate average later
        new_state->sum_temperature += temperature_val;

        // update max temperature if necessary
        if (temperature_val > new_state->max_temperature) {
            new_state->max_temperature = temperature_val;
            // update max temp timestamp
            new_state->max_temp_date = timestamp_long;
        }

        // update min temperature if necessary
        if (temperature_val < new_state->min_temperature) {
            new_state->min_temperature = temperature_val;
            // update min temp timestamp
            new_state->min_temp_date = timestamp_long;
        }
        // ----------------------------------------------------------
    }
}

void print_report(struct climate_info *states[], int num_states) {
    printf("States found:\n");
    int i;
    for (i = 0; i < num_states; ++i) {
        if (states[i] != NULL) {
            struct climate_info *info = states[i];
            printf("%s ", info->code);
        }
    }
    printf("\n");

    /* TODO: Print out the summary for each state. See format above. */
    for (i = 0; i < num_states; i++) {
        if (states[i] != NULL) {
            // print data in proper format
            printf(" -- State: %s --\n", states[i]->code);
            printf("Number of Records: %lu\n", states[i]->num_records);
            printf("Average Humidity: %.1Lf%%\n", (states[i]->sum_humidity) / states[i]->num_records);
            printf("Average Temperature: %.1LfF\n", (states[i]->sum_temperature) / states[i]->num_records);
            printf("Max Temperature: %.1lfF\n", states[i]->max_temperature);
            printf("Max Temperature on: %s", ctime(&states[i]->max_temp_date));
            printf("Min Temperature: %.1lfF\n", states[i]->min_temperature);
            printf("Min Temperature on: %s", ctime(&states[i]->min_temp_date));
            printf("Lightning Strikes: %lu\n", states[i]->num_lightning);
            printf("Records with Snow Cover: %lu\n", states[i]->num_snowcover);
            printf("Average Cloud Cover: %.1Lf%%\n", (states[i]->sum_cloudcover) / states[i]->num_records);
        }
    }
}
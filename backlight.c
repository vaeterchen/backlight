#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>

#define BACKLIGHT "/sys/class/backlight/"
#define F_MAX "/max_brightness"
#define F_ACT "/actual_brightness"
#define F_SET "/brightness"

const char *argp_program_verision = "1.0";
const char *argp_program_bug_address = "robert@uhlandstrasse.de";

static char doc[] = "Set backlight via /sys/class/backlight/...";
static char args_doc[] = "devicename (intel_backlight)";

struct argp_option options[] = {
	{"inc", 'i', "step", 0, "Increment brightness by step%"},
	{"dec", 'd', "step", 0, "Decrement brightness by step%"},
	{"set", 's', "value", 0, "Set brightness to value%"},
	{0}
};

struct arguments {
	int increment;
	int decrement;
	int value;
	char *device;
	int num_opts;
} args;

int get_bl_val(const char *device, const char *value)
{
	char *s = malloc(strlen(BACKLIGHT) 
			+ strlen(device) + strlen(value) + 1);
	if (!s) {
		perror("get_bl_value");
		exit(EXIT_FAILURE);
	}
	strcpy(s, BACKLIGHT);
	strcat(s, device);
	strcat(s, value);
	FILE *f = fopen(s, "r");
	if (NULL == f) {
		perror(s);
		exit(EXIT_FAILURE);
	}
	free(s);
	int n = -1;
	if (1 != fscanf(f, "%i", &n)) {
		exit(EXIT_FAILURE);
	}
	fclose(f);
	return n;
}

void set_bl_val(const char *device, const char *value, int val)
{
	char *s = malloc(strlen(BACKLIGHT) 
			+ strlen(device) + strlen(value) + 1);
	if (!s) {
		perror("get_bl_value");
		exit(EXIT_FAILURE);
	}
	strcpy(s, BACKLIGHT);
	strcat(s, device);
	strcat(s, value);
	FILE *f = fopen(s, "w");
	if (NULL == f) {
		perror(s);
		exit(EXIT_FAILURE);
	}
	free(s);
	if (1 != fprintf(f, "%i", val)) {
		exit(EXIT_FAILURE);
	}
	fclose(f);
}

static int parse_opt (int key, char *arg,
		 struct argp_state *state)
{
	struct arguments *args = state->input;
	switch (key) {
	case 'i': 
		if (args->increment) {
			argp_usage(state);
		}
		args->increment = atoi(arg);
		args->num_opts++;
		break;
	case 'd':
		args->decrement = atoi(arg);
		args->num_opts++;
		break;
	case 's':
		args->value = atoi(arg);
		args->num_opts++;
		break;
	case ARGP_KEY_ARG:
		if (state->arg_num > 0) {
			argp_usage(state);
		}
		args->device = arg;
		break;
	case ARGP_KEY_END:
		if (state->arg_num == 0) {
			argp_usage(state);
		}
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

struct argp argp = {
	options, 
	parse_opt,
	args_doc,
	doc
};

int main(int argc, char **argv)
{
	argp_parse (&argp, argc, argv, 0, 0, &args);
	/*
	printf("inc = %i\ndec = %i\nval = %i\ndev = %s\n", args.increment,
			args.decrement, args.value, args.device);
	*/
	int act = get_bl_val(args.device, F_ACT);
	int max = get_bl_val(args.device, F_MAX);
	int act_p = (int) (100.0f * ((float) act / (float) max));

	if (0 == args.num_opts) {
		printf("%i%%\n", act_p);
		return 0;
	}

	if (args.increment) {
		act_p += args.increment;
	} else if (args.decrement) {
		act_p -= args.decrement;
	} else if (args.value) {
		act_p = args.value;
	}

	act_p = act_p > 100 ? 100 : act_p;
	act_p = act_p < 0 ? 0 : act_p;
        act = (int) (((float) act_p * (float) max) / 100.0f); 
	//printf("set to %i, %i\n", act, act_p);
	set_bl_val(args.device, F_SET, act);
	return 0;
}

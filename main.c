#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <linux/joystick.h>
#include <stdio.h>
#include <bcm2835.h>

#include "js_ds4.h"
#include "main.h"

const char joystick[] = "/dev/input/js0";

char* js_button_name(u_int8_t key_id);
char* js_axis_name(u_int8_t key_id);
int hw_init(void);

void js_button_update(u_int8_t key_id, int16_t value, u_int16_t keys_states)
{
	switch (key_id)
	{
	case KEY_CROSS :
	case KEY_CIRCLE :
	case KEY_TRIANGLE :
	case KEY_SQUARE :
	case KEY_L1 :
	case KEY_R1 :
		return;
	case KEY_L2 :
		if (keys_states & BIT(KEY_R2))
			bcm2835_pwm_set_mode(PWM_CHANNEL, 1, (value ? 0 : 1));
		return;

	case KEY_R2 :
		if (keys_states & BIT(KEY_L2))
			bcm2835_pwm_set_mode(PWM_CHANNEL, 1, (value ? 0 : 1));
		return;
	case KEY_SHARE :
	case KEY_OPTIONS :
	case KEY_PS_LOGO :
	case KEY_L_ANALOG :
	case KEY_R_ANALOG :
	default :
		return;
	}
}

void js_axis_update(u_int8_t key_id, int16_t value, u_int16_t keys_states)
{
	switch (key_id)
	{
	case AXIS_LANALOG_X :
	case AXIS_LANALOG_Y :
	case AXIS_L2 :
	case AXIS_RANALOG_X :
	case AXIS_RANALOG_Y :
		return;
	case AXIS_R2 :
		bcm2835_pwm_set_data(PWM_CHANNEL, (u_int32_t)(value + JS_AXIS_MAX));
		return;
	case AXIS_DPAD_X :
	case AXIS_DPAD_Y :
	default :
		return;
	}
	
}

void js_read(int fd)
{
	struct js_event e;
	u_int16_t keys_states = 0;

	while (1){
		ssize_t len = read(fd, &e, sizeof(e));

		if (len < 0)
		{
			printf("Failed to read data\n");
			perror("Error");
			return;
		}

		if (len != sizeof(e))
			return;

		if (e.type == JS_EVENT_BUTTON)
		{
			//printf("There is %s button event!\n", js_button_name(e.number));

			if (e.value)
				keys_states |= BIT(e.number);
			else
				keys_states &= ~BIT(e.number);

			js_button_update(e.number, e.value, keys_states);

			//printf("Name: %s, state: %x\n", js_button_name(e.number), keys_states);
		}

		if (e.type == JS_EVENT_AXIS)
		{
			//printf("There is an %s event!\n", js_axis_name(e.number));
			//printf("Value: %d \n", e.value);
			js_axis_update(e.number, e.value, keys_states);
		}

		if (e.type & JS_EVENT_INIT)
			continue;
			//printf("Init: type: 0x%x number: %x \n", e.type, e.number);

	}

}

void js_init(int fd)
{
	u_int8_t axis_count;
	u_int8_t bttn_count;
	char js_name[256];

	ioctl(fd, JSIOCGAXES, &axis_count);
	ioctl(fd, JSIOCGBUTTONS, &bttn_count);
	ioctl(fd, JSIOCGNAME(sizeof(js_name)), js_name);

	//printf("Controller name: %s\n", js_name);
	//printf("Number of axes: %i\n", axis_count);
	//printf("Number of buttons: %i\n", bttn_count);

	js_read(fd);
}

int main(int argc, char **argv)
{
	int fd;

	fd = open(joystick, O_RDONLY);

	if (fd < 0)
	{
		printf("Failed to open %s \n", joystick);
		perror("Error");
		return 1;
	}

	if (hw_init())
	{
		printf("Failed hardware init");
		return 1;
	}

	js_init(fd);

	close(fd);

	return 0;
}

int hw_init(void)
{
	if (!bcm2835_init())
		return 1;

	bcm2835_gpio_fsel(PIN, BCM2835_GPIO_FSEL_ALT5);

	bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_2);
	bcm2835_pwm_set_mode(PWM_CHANNEL, 1, 1);
	bcm2835_pwm_set_range(PWM_CHANNEL, PWM_RANGE);

	return 0;
}

char* js_button_name(u_int8_t key_id)
{
	switch (key_id)
	{
	case 0 :
		return "CROSS";
	case 1 :
		return "CIRCLE";
	case 2 :
		return "TRIANGLE";
	case 3 :
		return "SQUARE";
	case 4 :
		return "left bumper (L1)";
	case 5 :
		return "right bumper (R1)";
	case 6 :
		return "left trigger (L2)";
	case 7 :
		return "right trigger (R2)";
	case 8 :
		return "share";
	case 9 :
		return "options";
	case 10 :
		return "PS logo";
	case 11 :
		return "left analog";
	case 12 :
		return "right analog";
	default :
		return "unknown";
	}
}

char* js_axis_name(u_int8_t key_id)
{
	switch (key_id)
	{
	case 0 :
		return "left analog x-axis";
	case 1 :
		return "left analog y-axis";
	case 2 :
		return "left trigger (L2)";
	case 3 :
		return "right analog x-axis";
	case 4 :
		return "right analog y-axis";
	case 5 :
		return "right trigger (R2)";
	case 6 :
		return "d-pad x-axis";
	case 7 :
		return "d-pad y-axis";
	default :
		return "unknown";
	}
}



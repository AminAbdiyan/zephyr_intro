#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

// Settings
static const int32_t sleep_time_ms = 100;
static const struct gpio_dt_spec btn = GPIO_DT_SPEC_GET(DT_ALIAS(my_button), gpios);
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_NODELABEL(led0), gpios);

void button_pressed(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
	gpio_pin_toggle_dt(&led);
}

/*  Define a variable of type static struct gpio_callback */
static struct gpio_callback button_cb_data;

int main(void)
{
    int ret;

    // Make sure that the button was initialized
    if (!gpio_is_ready_dt(&btn)) {
        printk("ERROR: button not ready\r\n");
        return 0;
    }

    // Set the button as input (apply extra flags if needed)
    ret = gpio_pin_configure_dt(&btn, GPIO_INPUT);
    if (ret < 0) {
        return 0;
    }

    // Make sure that the GPIO was initialized
	if (!gpio_is_ready_dt(&led)) {
		return 0;
	}

	// Set the GPIO as output
	ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT);
	if (ret < 0) {
		return 0;
	}

    /* Configure the interrupt on the button's pin */
	ret = gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);

    /*Initialize the static struct gpio_callback variable   */
	gpio_init_callback(&button_cb_data, button_pressed, BIT(btn.pin));

    /* Add the callback function to the button's GPIO device */
    gpio_add_callback(btn.port, &button_cb_data);


    // Print out the flags
    printk("Button spec flags: 0x%x\r\n", btn.dt_flags);

    // Do forever
    while (1) {
        // Sleep
        k_msleep(sleep_time_ms);
    }

    return 0;
}
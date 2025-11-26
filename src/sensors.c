#include "sensors.h"
#include "ipc.h"
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

LOG_MODULE_REGISTER(sensors, LOG_LEVEL_INF);

static const struct gpio_dt_spec sensor_gpio =
    GPIO_DT_SPEC_GET(DT_ALIAS(sensorgpio), gpios);

static const struct gpio_dt_spec camera_gpio =
    GPIO_DT_SPEC_GET(DT_ALIAS(cameragpio), gpios);

static struct gpio_callback cb_sensor;
static struct gpio_callback cb_camera;

static uint32_t t_sensor = 0;
static uint32_t t_camera = 0;
static uint8_t contagem_eixos = 0;


static void sensor_isr(const struct device *dev,
                       struct gpio_callback *cb,
                       uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    t_sensor = k_uptime_get_32();
    contagem_eixos = 1;
}

static void camera_isr(const struct device *dev,
                       struct gpio_callback *cb,
                       uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    t_camera = k_uptime_get_32();
    contagem_eixos++;

    struct sensor_msg msg;
    msg.num_eixos = contagem_eixos;
    msg.tempo_ms = (t_camera >= t_sensor) ? (t_camera - t_sensor) : 0;

    struct k_msgq *q = get_sensor_msgq();
    if (q) {
        k_msgq_put(q, &msg, K_NO_WAIT);
    }

    contagem_eixos = 0;  
}

void sensors_init(void)
{
    if (!device_is_ready(sensor_gpio.port) ||
        !device_is_ready(camera_gpio.port)) {
        LOG_ERR("GPIO dos sensores/câmera não está pronto!");
        return;
    }

    /* Configura pinos */
    gpio_pin_configure_dt(&sensor_gpio, GPIO_INPUT | GPIO_PULL_UP);
    gpio_pin_configure_dt(&camera_gpio, GPIO_INPUT | GPIO_PULL_UP);

    /* Interrupções */
    gpio_pin_interrupt_configure_dt(&sensor_gpio, GPIO_INT_EDGE_TO_ACTIVE);
    gpio_pin_interrupt_configure_dt(&camera_gpio, GPIO_INT_EDGE_TO_ACTIVE);

    /* Registra callbacks */
    gpio_init_callback(&cb_sensor, sensor_isr, BIT(sensor_gpio.pin));
    gpio_init_callback(&cb_camera, camera_isr, BIT(camera_gpio.pin));

    gpio_add_callback(sensor_gpio.port, &cb_sensor);
    gpio_add_callback(camera_gpio.port, &cb_camera);

    LOG_INF("Sensores inicializados (sensor=%d, camera=%d)",
            sensor_gpio.pin, camera_gpio.pin);
}

/*  Thread opcional                                           */
void sensors_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    LOG_INF("Thread de sensores iniciada.");
    while (1) {
        k_sleep(K_SECONDS(1));
    }
}

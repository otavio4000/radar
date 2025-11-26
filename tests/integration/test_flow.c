#include <zephyr/ztest.h>
#include "sensors.h"
#include "controller.h"
#include "ipc.h"

extern void sensor1_cb(const struct device *, struct gpio_callback *, uint32_t);
extern void sensor2_cb(const struct device *, struct gpio_callback *, uint32_t);

ZTEST(flow, test_veiculo_leve_fluxo)
{
    struct sensor_msg msg;

    /* Simula o primeiro sensor */
    sensor1_cb(NULL, NULL, BIT(5));
    k_sleep(K_MSEC(100));   // 100 ms depois

    /* Simula eixo extra */
    sensor1_cb(NULL, NULL, BIT(5));

    /* Simula o segundo sensor */
    sensor2_cb(NULL, NULL, BIT(6));

    /* Controller vai consumir via msgq */
    zassert_equal(k_msgq_get(&sensor_msgq, &msg, K_MSEC(50)), 0, "msg perdida");

    zassert_equal(msg.num_eixos, 2, "eixos incorretos");
    zassert_true(msg.tempo_ms > 0, "tempo errado");
}

ZTEST_SUITE(flow, NULL, NULL, NULL, NULL, NULL);

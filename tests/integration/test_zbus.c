#include <zephyr/ztest.h>
#include <zephyr/zbus/zbus.h>
#include "ipc.h"

ZTEST(zbus_test, test_camera_trigger_pubsub)
{
    struct camera_trigger trig = {
        .velocidade_kmh = 90.0,
        .tipo_veiculo = 0
    };

    zassert_equal(zbus_chan_pub(&camera_trigger_chan, &trig, K_NO_WAIT), 0, "pub falhou");

    struct camera_trigger recebido;
    zassert_equal(zbus_chan_sub_wait(&camera_trigger_chan, &recebido, K_MSEC(50)), 0, "sub falhou");

    zassert_equal(recebido.tipo_veiculo, 0, "tipo incorreto");
}

ZTEST_SUITE(zbus_test, NULL, NULL, NULL, NULL, NULL);

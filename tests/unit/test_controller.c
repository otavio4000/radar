#include <zephyr/ztest.h>
#include "src/controller.h"

ZTEST(calc_speed, test_speed_basic)
{
    int dist_mm = CONFIG_RADAR_SENSOR_DISTANCE_MM;
    uint32_t tempo_ms = 1000; // 1 segundo

    float v = controller_calc_speed_kmh(tempo_ms);

    float expected = (dist_mm / 1000.0f) * 3.6f;

    zassert_true(fabs(v - expected) < 0.1f, "velocidade incorreta");
}

ZTEST(classificacao, test_veiculo_leve)
{
    uint8_t tipo = controller_classificar_veiculo(2);
    zassert_equal(tipo, 0, "Veículo leve incorreto");
}

ZTEST(classificacao, test_veiculo_pesado)
{
    uint8_t tipo = controller_classificar_veiculo(3);
    zassert_equal(tipo, 1, "Veículo pesado incorreto");
}

ZTEST_SUITE(calc_speed, NULL, NULL, NULL, NULL, NULL);
ZTEST_SUITE(classificacao, NULL, NULL, NULL, NULL, NULL);

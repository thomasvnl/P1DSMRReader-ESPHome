#include "esphome.h"
#include "dsmr.h"

using namespace esphome;

using DsmrData = ParsedData <
  /* FixedValue */ energy_delivered_tariff1,
  /* FixedValue */ energy_delivered_tariff2,
  /* FixedValue */ energy_returned_tariff1,
  /* FixedValue */ energy_returned_tariff2,
  /* FixedValue */ electricity_tariff,
  /* FixedValue */ power_delivered,
  /* FixedValue */ power_returned,
  /* FixedValue */ voltage_l1,
  /* FixedValue */ voltage_l2,
  /* FixedValue */ voltage_l3,
  /* FixedValue */ current_l1,
  /* FixedValue */ current_l2,
  /* FixedValue */ current_l3,
  /* FixedValue */ //power_delivered_l1,
  /* FixedValue */ //power_delivered_l2,
  /* FixedValue */ //power_delivered_l3,
  /* FixedValue */ //power_returned_l1,
  /* FixedValue */ //power_returned_l2,
  /* FixedValue */ //power_returned_l3,
  /* TimestampedFixedValue */ gas_delivered
>;

#define DSMR_VALID_TIMESTAMP_LENGTH 13U

class UARTP1ReaderComponent : public PollingComponent {
	private:
		P1Reader *reader;
	public: 
		Sensor *energy_delivered_tariff1 = new Sensor();
		Sensor *energy_delivered_tariff2 = new Sensor();
		Sensor *energy_returned_tariff1 = new Sensor();
		Sensor *energy_returned_tariff2 = new Sensor();
		Sensor *electricity_tariff = new Sensor();
		Sensor *power_delivered = new Sensor();
		Sensor *power_returned = new Sensor();
		Sensor *voltage_l1 = new Sensor();
		Sensor *voltage_l2 = new Sensor();
		Sensor *voltage_l3 = new Sensor();
		Sensor *current_l1 = new Sensor();
		Sensor *current_l2 = new Sensor();
		Sensor *current_l3 = new Sensor();
		Sensor *power_delivered_l1 = new Sensor();
		Sensor *power_delivered_l2 = new Sensor();
		Sensor *power_delivered_l3 = new Sensor();
		Sensor *gas_delivered = new Sensor();

		UARTP1ReaderComponent(UARTComponent *uart_bus) : PollingComponent(500) {

			this->reader = new P1Reader(uart_bus, -1);
		}

		void setup() override {
			this->reader->enable(false);
		}

		void update() override {
			reader->loop();
			// Check if data available
			if (reader->available()) {
				DsmrData data;
				String parseError;
				if (reader->parse(&data, &parseError)) {
					if (data.energy_delivered_tariff1_present) { energy_delivered_tariff1->publish_state(data.energy_delivered_tariff1); }
					if (data.energy_delivered_tariff2_present) { energy_delivered_tariff2->publish_state(data.energy_delivered_tariff2); }
					if (data.energy_returned_tariff1_present) { energy_returned_tariff1->publish_state(data.energy_returned_tariff1); }
					if (data.energy_returned_tariff2_present) { energy_returned_tariff2->publish_state(data.energy_returned_tariff2); }
					if (data.electricity_tariff_present) {
						if (data.electricity_tariff == "0002") {
							electricity_tariff->publish_state(2);
						} else {
							electricity_tariff->publish_state(1);
						}
					}
					if (data.power_delivered_present) { power_delivered->publish_state(data.power_delivered); }
					if (data.power_returned_present) { power_returned->publish_state(data.power_returned); }
					if (data.gas_delivered_present) { gas_delivered->publish_state(data.gas_delivered); }
					if (data.voltage_l1_present) { voltage_l1->publish_state(data.voltage_l1); }
					if (data.voltage_l2_present) { voltage_l2->publish_state(data.voltage_l2); }
					if (data.voltage_l3_present) { voltage_l3->publish_state(data.voltage_l3); }
					if (data.current_l1_present) { current_l1->publish_state(data.current_l1); }
					if (data.current_l2_present) { current_l2->publish_state(data.current_l2); }
					if (data.current_l3_present) { current_l3->publish_state(data.current_l3); }

				} else {
					ESP_LOGE("P1Reader", parseError.c_str());
				}
			}
		}

};
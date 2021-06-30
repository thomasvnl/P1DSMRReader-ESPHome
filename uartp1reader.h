#include "esphome.h"
#include "dsmr.h"

using namespace esphome;

using DsmrData = ParsedData <
	/* String */   p1_version,
  /* String */     timestamp,
  /* FixedValue */ energy_delivered_tariff1, 
  /* FixedValue */ energy_delivered_tariff2, 
  /* FixedValue */ energy_returned_tariff1, 
  /* FixedValue */ energy_returned_tariff2, 
  /* String */ 	   electricity_tariff,
  /* FixedValue */ power_delivered,
  /* FixedValue */ power_returned,
  /* FixedValue */ voltage_l1,
  /* FixedValue */ voltage_l2,
  /* FixedValue */ voltage_l3,
  /* FixedValue */ current_l1,
  /* FixedValue */ current_l2,
  /* FixedValue */ current_l3,
  /* FixedValue */ power_delivered_l1,
  /* FixedValue */ power_delivered_l2,
  /* FixedValue */ power_delivered_l3,
  /* FixedValue */ power_returned_l1,
  /* FixedValue */ power_returned_l2,
  /* FixedValue */ power_returned_l3,
  /* TimestampedFixedValue */ gas_delivered
>;

#define DSMR_VALID_TIMESTAMP_LENGTH 13U

/**
 * @brief	This component reads and parses P1 DSMR data. It allows for some configuration by giving the constructor
 * 			either a (Text)Sensor or a null pointer for the different data that can be extracted from the meter.
 * 			When given a null pointer, the data will be discarded. When given a (Text)Sensor, the data will
 * 			be published via the method configured in the esphome config for your device.
 * 				
 * 			Example usage in the esphome config yaml where we ignore the dsmr version sensor. id(object_name) refers to
 * 			objects in your esphome configuration, just like id(uart_bus) refers to the uart perhipheral configured. The
 * 			three dots indicate other sensors are configured.
 * @code
 *			auto p1sensor = UARTP1ReaderComponent(id(uart_bus), NULL, id(dsmr_timestamp),...);
 * @endcode
 */
class UARTP1ReaderComponent : public PollingComponent {
	private:
		P1Reader *reader;
		UARTComponent *uart_bus;

		/**
		 * @brief	The version of the P1/DSMR standard used by the meter
		 */
		TextSensor *xdsmr_version {nullptr};
		/**
		 * @brief	The timestamp for electricity related measurements.
		 */
		TextSensor *xdsmr_timestamp {nullptr};
		
		/**
		 * @brief	The current electricity tariff. 1 = low, 2 = normal/peak
		 */
		Sensor *xelectricity_tariff {nullptr};
		/**
		 * @brief	Accumulated meter reading for total elecricity consumed low tariff
		 */
		Sensor *xelectricity_consumed_tariff1 {nullptr};
		/**
		 * @brief	Accumulated meter reading for total electricity consumed normal/peak tariff
		 */
		Sensor *xelectricity_consumed_tariff2 {nullptr};
		/**
		 * @brief	Accumulated meter reading for total electricity produced low tariff
		 */
		Sensor *xelectricity_produced_tariff1 {nullptr};
		/**
		 * @brief	Accumulated meter reading for total electricity produced normal/peak tariff
		 */
		Sensor *xelectricity_produced_tariff2 {nullptr};
		/**
		 * @brief	The actual power consumed from the grid
		 */
		Sensor *xactual_power_consumed {nullptr};
		/**
		 * @brief	The actual power delivered to the grid
		 */
		Sensor *xactual_power_produced {nullptr};
		/** 
		 * @brief	Timestamp of last total gas consumed measurement
		 */
		TextSensor *xgas_timestamp {nullptr};
		/**
		 * @brief	Accumulated meter reading for total gas consumed
		 */
		Sensor *xgas_consumed {nullptr};

		///@{
		/**
		 * @brief 	Actual power consumed on phase l(x) where x describes which phase it is.
		 */
		Sensor *xpower_consumed_l1 {nullptr};
		Sensor *xpower_consumed_l2 {nullptr};
		Sensor *xpower_consumed_l3 {nullptr};
		///@}

		///@{
		/**
		 * @brief	Actual power returned on phase l(x) where x describes which phase it is.
		 */
		Sensor *xpower_produced_l1 {nullptr};
		Sensor *xpower_produced_l2 {nullptr};
		Sensor *xpower_produced_l3 {nullptr};
		///@}

		///@{
		/**
		 * @brief	Voltage on phase l(x) where x describes which phase it is.
		 */
		Sensor *xvoltage_l1 {nullptr};
		Sensor *xvoltage_l2 {nullptr};
		Sensor *xvoltage_l3 {nullptr};
		///@}

		///@{
		/**
		 * @brief	Current on phase l(x) where x describes which phase it is.
		 * @note	Please note that currents from phases have a resolultion of 1 ampere. This makes
		 * 			them pretty useless. Its better to use voltage and power consumed/procuded per
		 * 			phase to accurately calculate current if necessary.
		 */
		Sensor *xcurrent_l1 {nullptr};
		Sensor *xcurrent_l2 {nullptr};
		Sensor *xcurrent_l3 {nullptr};
		///@}

	public: 
		UARTP1ReaderComponent(
			UARTComponent *uart_bus,
			TextSensor *p_dsmr_version, TextSensor *p_dsmr_timestamp,
			Sensor *p_electricity_tariff,
			Sensor *p_electricity_consumed_tariff1, Sensor *p_electricity_consumed_tariff2,
			Sensor *p_electricity_produced_tariff1, Sensor *p_electricity_produced_tariff2,
			Sensor *p_actual_power_consumed, Sensor *p_actual_power_produced,
			TextSensor *p_gas_timestamp, Sensor *p_gas_consumed,
			Sensor *p_power_consumed_l1, Sensor *p_power_consumed_l2, Sensor *p_power_consumed_l3,
			Sensor *p_power_produced_l1, Sensor *p_power_produced_l2, Sensor *p_power_produced_l3,
			Sensor *p_voltage_l1, Sensor *p_voltage_l2, Sensor *p_voltage_l3,
			Sensor *p_current_l1, Sensor *p_current_l2, Sensor *p_current_l3
		) : PollingComponent(500) {

			this->uart_bus = uart_bus;
			this->reader = new P1Reader(uart_bus, -1);
			this->xdsmr_version = p_dsmr_version;
			this->xdsmr_timestamp = p_dsmr_timestamp;
			this->xelectricity_tariff = p_electricity_tariff;
			this->xelectricity_consumed_tariff1 = p_electricity_consumed_tariff1;
			this->xelectricity_consumed_tariff2 = p_electricity_consumed_tariff2;
			this->xelectricity_produced_tariff1 = p_electricity_produced_tariff1;
			this->xelectricity_produced_tariff2 = p_electricity_produced_tariff2;
			this->xactual_power_consumed = p_actual_power_consumed;
			this->xactual_power_produced = p_actual_power_produced;
			this->xgas_timestamp = p_gas_timestamp;
			this->xgas_consumed = p_gas_consumed;
			this->xpower_consumed_l1 = p_power_consumed_l1;
			this->xpower_consumed_l2 = p_power_consumed_l2;
			this->xpower_consumed_l3 = p_power_consumed_l3;
			this->xpower_produced_l1 = p_power_produced_l1;
			this->xpower_produced_l2 = p_power_produced_l2;
			this->xpower_produced_l3 = p_power_produced_l3;
			this->xvoltage_l1 = p_voltage_l1;
			this->xvoltage_l2 = p_voltage_l2;
			this->xvoltage_l3 = p_voltage_l3;
			this->xcurrent_l1 = p_current_l1;
			this->xcurrent_l2 = p_current_l2;
			this->xcurrent_l3 = p_current_l3;
		}

		void setup() override {
			reader->enable(false);

		}

		void update() override {
			if (digitalRead(4) == HIGH && uart_bus->available() > 0) {
				reader->loop();
			}

			// Check if data available
			if (reader->available()) {
				DsmrData data;
				String parseError;
				if (reader->parse(&data, &parseError)) {	

					if (nullptr != xdsmr_version && data.p1_version_present) {
						xdsmr_version->state = data.p1_version.c_str();
					}

					if (nullptr != xdsmr_timestamp && data.timestamp_present) {
						xdsmr_timestamp->publish_state(data.timestamp.c_str());
					}

					if (nullptr != xelectricity_tariff && data.electricity_tariff_present) {
						if (data.electricity_tariff == "0002") {
							xelectricity_tariff->publish_state(2);
						}
						else if(data.electricity_tariff == "0001") {
							xelectricity_tariff->publish_state(1);
						}
					}

					if (nullptr != xelectricity_consumed_tariff1 && data.energy_delivered_tariff1_present){
						xelectricity_consumed_tariff1->publish_state(data.energy_delivered_tariff1.int_val());
					}

					if (nullptr != xelectricity_consumed_tariff2 && data.energy_delivered_tariff2_present) {
						xelectricity_consumed_tariff2->publish_state(data.energy_delivered_tariff2.int_val());
					}

					if (nullptr != xelectricity_produced_tariff1 && data.energy_returned_tariff1_present) {
						xelectricity_produced_tariff1->publish_state(data.energy_returned_tariff1.int_val());
					}

					if (nullptr != xelectricity_produced_tariff2 && data.energy_returned_tariff2_present) {
						xelectricity_produced_tariff2->publish_state(data.energy_returned_tariff2.int_val());
					}

					if (nullptr != xactual_power_consumed && data.power_delivered_present) {
						xactual_power_consumed->publish_state(data.power_delivered.int_val());
					}

					if (nullptr != xactual_power_produced && data.power_returned_present) {
						xactual_power_produced->publish_state(data.power_returned.int_val());
					}

					if (nullptr != xgas_consumed && data.gas_delivered_present) {
						xgas_consumed->publish_state(data.gas_delivered.int_val());
					}

					if (nullptr != xgas_timestamp && data.gas_delivered_present) {
						xgas_timestamp->publish_state(data.gas_delivered.timestamp.c_str());
					}

					if (nullptr != xpower_consumed_l1 && data.power_delivered_l1_present) {
						xpower_consumed_l1->publish_state(data.power_delivered_l1.int_val());
					}

					if (nullptr != xpower_consumed_l2 && data.power_delivered_l2_present) {
						xpower_consumed_l2->publish_state(data.power_delivered_l2.int_val());
					}

					if (nullptr != xpower_consumed_l3 && data.power_delivered_l3_present) {
						xpower_consumed_l3->publish_state(data.power_delivered_l3.int_val());
					}

					if (nullptr != xpower_produced_l1 && data.power_returned_l1_present) {
						xpower_produced_l1->publish_state(data.power_returned_l1.int_val());
					}

					if (nullptr != xpower_produced_l2 && data.power_returned_l2_present) {
						xpower_produced_l2->publish_state(data.power_returned_l2.int_val());
					}

					if (nullptr != xpower_produced_l3 && data.power_returned_l3_present) {
						xpower_produced_l3->publish_state(data.power_returned_l3.int_val());
					}

					if (nullptr != xvoltage_l1 && data.voltage_l1_present) {
						xvoltage_l1->publish_state(data.voltage_l1.val());
					}

					if (nullptr != xvoltage_l2 && data.voltage_l2_present) {
						xvoltage_l2->publish_state(data.voltage_l2.val());
					}

					if (nullptr != xvoltage_l3 && data.voltage_l3_present) {
						xvoltage_l3->publish_state(data.voltage_l3.val());
					}

					if (nullptr != xcurrent_l1 && data.current_l1_present) {
						xcurrent_l1->publish_state(data.current_l1);
					}

					if (nullptr != xcurrent_l2 && data.current_l2_present) {
						xcurrent_l2->publish_state(data.current_l2);
					}

					if (nullptr != xcurrent_l3 && data.current_l3_present) {
						xcurrent_l3->publish_state(data.current_l3);
					}

				} else {
					ESP_LOGE("UARTP1ReaderComponent", parseError.c_str());
				}
			}
		}

};
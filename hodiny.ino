// lower bits come first
const static int pins_hA[2] = { A5, A4 };
const static int pins_hB[4] = { A3, A2, A1, A0 };
const static int pins_mA[3] = { 12, 11, 10 };
const static int pins_mB[4] = { 9, 8, 7, 6 };
const static int pin_s = 5; // pwm
const static int pinbtn_h = 3;
const static int pinbtn_m = 4;
const static int pin_ledA = 13;
const static int pin_ledB = 2;

const static int second_length = 1000; // [ms], <1000 to make time go fast

static void init_pins() {
	pinMode(pins_hA[0], OUTPUT); digitalWrite(pins_hA[0], LOW);
	pinMode(pins_hA[1], OUTPUT); digitalWrite(pins_hA[1], LOW);
	pinMode(pins_hB[0], OUTPUT); digitalWrite(pins_hB[0], LOW);
	pinMode(pins_hB[1], OUTPUT); digitalWrite(pins_hB[1], LOW);
	pinMode(pins_hB[2], OUTPUT); digitalWrite(pins_hB[2], LOW);
	pinMode(pins_hB[3], OUTPUT); digitalWrite(pins_hB[3], LOW);

	pinMode(pins_mA[0], OUTPUT); digitalWrite(pins_hA[0], LOW);
	pinMode(pins_mA[1], OUTPUT); digitalWrite(pins_hA[1], LOW);
	pinMode(pins_mA[2], OUTPUT); digitalWrite(pins_hA[1], LOW);
	pinMode(pins_mB[0], OUTPUT); digitalWrite(pins_mB[0], LOW);
	pinMode(pins_mB[1], OUTPUT); digitalWrite(pins_mB[1], LOW);
	pinMode(pins_mB[2], OUTPUT); digitalWrite(pins_mB[2], LOW);
	pinMode(pins_mB[3], OUTPUT); digitalWrite(pins_mB[3], LOW);

	pinMode(pin_s, OUTPUT); digitalWrite(pin_s, LOW);
	pinMode(pin_ledA, OUTPUT); digitalWrite(pin_ledA, LOW);
	pinMode(pin_ledB, OUTPUT); digitalWrite(pin_ledB, LOW);

	pinMode(pinbtn_h, OUTPUT); digitalWrite(pinbtn_h, HIGH);
	pinMode(pinbtn_m, OUTPUT); digitalWrite(pinbtn_m, HIGH);
}

void write_digit_bits(unsigned char digit, const int pins[], int n_pins) {
	int i;
	for (i = 0; i < n_pins; i++)
		digitalWrite(pins[i], (digit >> i) & 1);
}


struct Time {
	int h, m, s, ms;
	unsigned long last_millis;

	Time() : h(0), m(0), s(0), ms(0), last_millis(0) {
		last_millis = millis();
	}

	void tick() {
		unsigned long new_millis = millis();
		ms += new_millis - last_millis;
		last_millis = new_millis;

		while (ms >= second_length)
			ms -= second_length, s += 1;
		if (s >= 60)
			s -= 60, m += 1;
		if (m >= 60)
			m -= 60, h += 1;
		if (h >= 24)
			h -= 24;
	}

	void display_time() {
		write_digit_bits(h / 10, pins_hA, 2);
		write_digit_bits(h % 10, pins_hB, 4);

		write_digit_bits(m / 10, pins_mA, 3);
		write_digit_bits(m % 10, pins_mB, 4);

		digitalWrite(pin_ledA, s % 2);
	}
};

Time t;


// sw debounce: if HIGH counter >50
unsigned long m_btn_debounce, h_btn_debounce;
const static int debounce = 50, repeat = 450; // [ms]

bool m_btn_is_pressed()
{
	unsigned char state = digitalRead(pinbtn_m);
	if (state == LOW) {
		m_btn_debounce++;
		if (m_btn_debounce == debounce) {
			return true;
		} else if (m_btn_debounce >= repeat) {
			m_btn_debounce = 0;
		}
	} else {
		m_btn_debounce = 0;
	}
	return false;
}

void handle_m_btn()
{
	if (!m_btn_is_pressed())
		return;
	t.m = (t.m + 1) % 60;
}


bool h_btn_is_pressed()
{
	unsigned char state = digitalRead(pinbtn_h);
	if (state == LOW) {
		h_btn_debounce++;
		if (h_btn_debounce == debounce) {
			return true;
		} else if (h_btn_debounce >= repeat) {
			h_btn_debounce = 0;
		}
	} else {
		h_btn_debounce = 0;
	}
	return false;
}

void handle_h_btn()
{
	if (!h_btn_is_pressed())
		return;
	t.h += 1;
}


void setup(void)
{
	init_pins();

	Serial.begin(9600);
	Serial.println("booted up");
}

void loop(void)
{
	t.tick();
	t.display_time();

	handle_m_btn();
	handle_h_btn();

	delay(1);
}

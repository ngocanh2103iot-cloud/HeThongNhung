#define DIN 12
#define CS  11
#define CLK 10

// Thanh ghi MAX7219
#define REG_NOOP        0x00
#define REG_DIGIT0      0x01
#define REG_DIGIT1      0x02
#define REG_DIGIT2      0x03
#define REG_DIGIT3      0x04
#define REG_DIGIT4      0x05
#define REG_DIGIT5      0x06
#define REG_DIGIT6      0x07
#define REG_DIGIT7      0x08
#define REG_DECODEMODE  0x09
#define REG_INTENSITY   0x0A
#define REG_SCANLIMIT   0x0B
#define REG_SHUTDOWN    0x0C
#define REG_DISPLAYTEST 0x0F

// Trái tim đặc
byte heart[8] = {
  B00000000,
  B01100110,
  B11111111,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011000
};

void writeByte(byte data) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(CLK, LOW);
    digitalWrite(DIN, (data >> i) & 0x01);
    digitalWrite(CLK, HIGH);
  }
}

void writeReg(byte addr, byte data) {
  digitalWrite(CS, LOW);
  writeByte(addr);
  writeByte(data);
  digitalWrite(CS, HIGH);
}

void max7219Init() {
  writeReg(REG_DISPLAYTEST, 0x00); // tắt kiểm tra
  writeReg(REG_DECODEMODE,  0x00); // không giải mã
  writeReg(REG_SCANLIMIT,   0x07); // dùng đủ 8 hàng
  writeReg(REG_INTENSITY,   0x08); // sáng 0–15
  writeReg(REG_SHUTDOWN,    0x01); // bật hiển thị
  for (int i = 1; i <= 8; i++) writeReg(i, 0x00); // xóa màn
}

void drawHeart() {
  for (int row = 0; row < 8; row++) {
    writeReg(row + 1, heart[row]);  // DIGIT0..DIGIT7 = hàng 1..8
  }
}

void setup() {
  pinMode(DIN, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(CLK, OUTPUT);
  digitalWrite(CS, HIGH);
  max7219Init();
  drawHeart();
}

void loop() {
  // giữ nguyên hình
}

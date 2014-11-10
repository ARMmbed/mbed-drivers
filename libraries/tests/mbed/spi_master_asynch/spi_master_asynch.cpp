#if DEVICE_SPI


#include <CppUTest/TestHarness.h>
#include <mbed.h>
#include <SPI.h>
#include <spi_api.h>

#define SHORT_XFR 3
#define LONG_XFR 16
#define TEST_BYTE0 0x00
#define TEST_BYTE1 0x11
#define TEST_BYTE2 0xFF
#define TEST_BYTE3 0xAA
#define TEST_BYTE4 0x55
#define TEST_BYTE5 0x50

#define TEST_BYTE_RX TEST_BYTE3
#define TEST_BYTE_TX_BASE TEST_BYTE5

#define TEST_MOSI_PIN p5
#define TEST_MISO_PIN p6
#define TEST_SCLK_PIN p7
#define TEST_CS_PIN   p8

TEST_GROUP(SPI_Master_Asynchronous)
{
	char tx_buf[LONG_XFR];
	char rx_buf[LONG_XFR];
	SPI *obj;
	DigitalOut *cs;
	volatile bool complete;
	volatile uint32_t why;

	void setup() {
		obj = new SPI(TEST_MOSI_PIN, TEST_MISO_PIN, TEST_SCLK_PIN);
		cs = new DigitalOut(TEST_CS_PIN);
		complete = false;
		// Set the default value of tx_buf
		for (i = 0; i < sizeof(tx_buf); i++) {
			tx_buf[i] = i + TEST_BYTE_TX_BASE;
		}
		memset(rx_buf,TEST_BYTE_RX,sizeof(rx_buf));
	}
	void teardown() {
		delete obj;
		obj = NULL;
		delete cs;
		cs = NULL;
	}

	void cbdone(uint32_t event) {
		complete = true;
		why = event;
	}

};

// SPI write tx length: FIFO-1, read length: 0
//   Checks: Null pointer exceptions, completion event
TEST(SPI_Master_Asynchronous, short_tx_0_rx)
{
	int rc;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,NULL,0,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// TODO: Check for a null pointer exception
}


//
// SPI write tx length: FIFO-1, read length: 0, non-null read pointer
//   Checks: Null pointer exceptions, completion event, canary values in read buffer
TEST(SPI_Master_Asynchronous, short_tx_0_rx_nn)
{
	int rc;
	int i;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,0,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the rx buffer is untouched
	for(i = 0; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}

// SPI write tx length: 0, read length: FIFO-1
//   Checks: Receive value==fill character, completion event
TEST(SPI_Master_Asynchronous, 0_tx_short_rx)
{
	int rc;
	int i;
	// Read a buffer of Short Transfer length.
	rc = obj->write(NULL,0,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// TODO: Check for null pointer exception
	// Check that the receive buffer contains the fill byte.
	for(i = 0; i < SHORT_XFR; i++) {
		CHECK(rx_buf[i] == SPI_FILL_BYTE);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}

// SPI write tx length: 0, read length: FIFO-1
//   Checks: Receive value==fill character, completion event
TEST(SPI_Master_Asynchronous, 0_tx_nn_short_rx)
{
	int rc;
	int i;
	// Read a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,0,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the receive buffer contains the fill byte.
	for(i = 0; i < SHORT_XFR; i++) {
		CHECK(rx_buf[i] == SPI_FILL_BYTE);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}

// SPI write tx length: FIFO-1 ascending values, read length: FIFO-1
//   Checks: Receive buffer == tx buffer, completion event
TEST(SPI_Master_Asynchronous, short_tx_short_rx)
{
	int rc;
	int i;
	// Write/Read a buffer of Long Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the rx buffer contains tx bytes
	for(i = 0; i < SHORT_XFR; i++) {
		CHECK(rx_buf[i] == tx_buf[i]);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}
// SPI write tx length: 2xFIFO ascending values, read length: 2xFIFO
//   Checks: Receive buffer == tx buffer, completion event
TEST(SPI_Master_Asynchronous, long_tx_long_rx)
{
	int rc;
	int i;
	// Write/Read a buffer of Long Transfer length.
	rc = obj->write((void *)tx_buf,LONG_XFR,(void *)rx_buf,LONG_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the rx buffer contains tx bytes
	for(i = 0; i < LONG_XFR; i++) {
		CHECK(rx_buf[i] == tx_buf[i]);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}
// SPI write tx length: 2xFIFO, ascending, read length: 1xFIFO
//   Checks: Receive buffer == tx buffer, completion event, read buffer overflow
TEST(SPI_Master_Asynchronous, long_tx_short_rx)
{
	int rc;
	int i;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,LONG_XFR,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the rx buffer contains the tx bytes
	for(i = 0; i < SHORT_XFR; i++) {
		CHECK(rx_buf[i] == tx_buf[i]);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}

// SPI write tx length: 1xFIFO, ascending, read length: 2xFIFO
//	 Checks: Receive buffer == tx buffer, then fill, completion event
TEST(SPI_Master_Asynchronous, short_tx_long_rx)
{
	int rc;
	int i;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,LONG_XFR,-1,cbdone);
	CHECK(rc == 0);

	while (!complete);

	// Make sure that the callback fires.
	CHECK(why == SPI_EVENT_COMPLETE);

	// Check that the rx buffer contains the tx bytes
	for(i = 0; i < SHORT_XFR; i++) {
		CHECK(rx_buf[i] == tx_buf[i]);
	}
	// Check that the rx buffer contains the tx fill bytes
	for(; i < LONG_XFR; i++) {
		CHECK(rx_buf[i] == SPI_FILL_BYTE);
	}
	// Check that remaining portion of the receive buffer contains the rx test byte
	for(; i < sizeof(rx_buf); i++) {
		CHECK(rx_buf[i] == TEST_BYTE_RX);
	}
}

// On DMA-enabled platforms, add an additional test with large transfers and DMA.
// To validate DMA, disable non-completion IRQs after starting the transfer.






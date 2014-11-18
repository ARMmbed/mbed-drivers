
#include <TestHarness.h>
#include <mbed.h>
#include <SPI.h>
#include <spi_api.h>
#include <stdio.h>

#if !DEVICE_SPI
#error spi_master_asynch requires SPI
#endif


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

#if defined(TARGET_K64F)
#define TEST_MOSI_PIN PTD2
#define TEST_MISO_PIN PTD3
#define TEST_SCLK_PIN PTD1
#define TEST_CS_PIN   PTD0
#else
#error Target not supported
#endif

volatile uint32_t  why;
volatile bool complete;
void cbdone(uint32_t event) {
	complete = true;
	why = event;
}


TEST_GROUP(SPI_Master_Asynchronous)
{
	uint8_t tx_buf[LONG_XFR];
	uint8_t rx_buf[LONG_XFR];
	SPI *obj;
	DigitalOut *cs;

	void setup() {
		obj = new SPI(TEST_MOSI_PIN, TEST_MISO_PIN, TEST_SCLK_PIN);
		cs = new DigitalOut(TEST_CS_PIN);
		complete = false;
		why = 0;

		// Set the default value of tx_buf
		for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
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
	uint32_t cmpnbuf(uint8_t *expect, uint8_t *actual, uint32_t offset, uint32_t end, const char *file, uint32_t line)
	{
		uint32_t i;
		for (i = offset; i < end; i++){
			if (expect[i] != actual[i]) {
				break;
			}
		}
		if (i < end) {
			CHECK_EQUAL_LOCATION((int)expect[i], (int)actual[i], file, line);
		}
		CHECK_EQUAL_LOCATION(end, i, file, line);
		return i;
	}
	uint32_t cmpnbufc(uint8_t expect, uint8_t *actual, uint32_t offset, uint32_t end, const char *file, uint32_t line)
	{
		uint32_t i;
		for (i = offset; i < end; i++){
			if (expect != actual[i]) {
				break;
			}
		}
		if (i < end) {
			CHECK_EQUAL_LOCATION((int)expect, (int)actual[i], file, line);
		}
		CHECK_EQUAL_LOCATION(end, i, file, line);
		return i;
	}
	void dumpRXbuf() {
		uint32_t i;
		printf("\r\n");
		printf("RX Buffer Contents: [");
		//flushf(stdout);
		for (i = 0; i < sizeof(rx_buf); i++){
			printf("%02x",rx_buf[i]);
			if (i+1 < sizeof(rx_buf)){
				printf(",");
			}
		}
		printf("]\r\n");
	}
};

// SPI write tx length: FIFO-1, read length: 0
//   Checks: Null pointer exceptions, completion event
TEST(SPI_Master_Asynchronous, short_tx_0_rx)
{
	int rc;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,NULL,0,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(why, SPI_EVENT_COMPLETE);

	// TODO: Check for a null pointer exception
}


//
// SPI write tx length: FIFO-1, read length: 0, non-null read pointer
//   Checks: Null pointer exceptions, completion event, canary values in read buffer
TEST(SPI_Master_Asynchronous, short_tx_0_rx_nn)
{
	int rc;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,0,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	// Check that the rx buffer is untouched
	cmpnbufc(TEST_BYTE_RX,rx_buf,0,sizeof(rx_buf),__FILE__,__LINE__);
}

// SPI write tx length: 0, read length: FIFO-1
//   Checks: Receive value==fill character, completion event
TEST(SPI_Master_Asynchronous, 0_tx_short_rx)
{
	int rc;
	// Read a buffer of Short Transfer length.
	rc = obj->write(NULL,0,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	// TODO: Check for null pointer exception
	// Check that the receive buffer contains the fill byte.
	cmpnbufc(SPI_FILL_WORD,rx_buf,0,SHORT_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,SHORT_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}

// SPI write tx length: 0, read length: FIFO-1
//   Checks: Receive value==fill character, completion event
TEST(SPI_Master_Asynchronous, 0_tx_nn_short_rx)
{
	int rc;
	// Read a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,0,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	// Check that the receive buffer contains the fill byte.
	cmpnbufc(SPI_FILL_WORD,rx_buf,0,SHORT_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,SHORT_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}

// SPI write tx length: FIFO-1 ascending values, read length: FIFO-1
//   Checks: Receive buffer == tx buffer, completion event
TEST(SPI_Master_Asynchronous, short_tx_short_rx)
{
	int rc;
	// Write/Read a buffer of Long Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	// Check that the rx buffer contains the tx bytes
	cmpnbuf(tx_buf,rx_buf,0,SHORT_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,SHORT_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}
// SPI write tx length: 2xFIFO ascending values, read length: 2xFIFO
//   Checks: Receive buffer == tx buffer, completion event
TEST(SPI_Master_Asynchronous, long_tx_long_rx)
{
	int rc;
	// Write/Read a buffer of Long Transfer length.
	rc = obj->write((void *)tx_buf,LONG_XFR,(void *)rx_buf,LONG_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	//dumpRXbuf();
	// Check that the rx buffer contains the tx bytes
	cmpnbuf(tx_buf,rx_buf,0,LONG_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,LONG_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}
// SPI write tx length: 2xFIFO, ascending, read length: FIFO-1
//   Checks: Receive buffer == tx buffer, completion event, read buffer overflow
TEST(SPI_Master_Asynchronous, long_tx_short_rx)
{
	int rc;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,LONG_XFR,(void *)rx_buf,SHORT_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	// Check that the rx buffer contains the tx bytes
	cmpnbuf(tx_buf,rx_buf,0,SHORT_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,SHORT_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}

// SPI write tx length: FIFO-1, ascending, read length: 2xFIFO
//	 Checks: Receive buffer == tx buffer, then fill, completion event
TEST(SPI_Master_Asynchronous, short_tx_long_rx)
{
	int rc;
	// Write a buffer of Short Transfer length.
	rc = obj->write((void *)tx_buf,SHORT_XFR,(void *)rx_buf,LONG_XFR,-1,cbdone);
	CHECK_EQUAL(0, rc);

	while (!complete);

	// Make sure that the callback fires.
	CHECK_EQUAL(SPI_EVENT_COMPLETE, why);

	//dumpRXbuf();
	// Check that the rx buffer contains the tx bytes
	cmpnbuf(tx_buf,rx_buf,0,SHORT_XFR,__FILE__,__LINE__);
	// Check that the rx buffer contains the tx fill bytes
	cmpnbufc(SPI_FILL_WORD,rx_buf,SHORT_XFR,LONG_XFR,__FILE__,__LINE__);
	// Check that remaining portion of the receive buffer contains the rx test byte
	cmpnbufc(TEST_BYTE_RX,rx_buf,LONG_XFR,sizeof(rx_buf),__FILE__,__LINE__);
}

// On DMA-enabled platforms, add an additional test with large transfers and DMA.
// To validate DMA, disable non-completion IRQs after starting the transfer.

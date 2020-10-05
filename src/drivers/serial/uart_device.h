/*
 * @file
 *
 * @date Oct 26, 2012
 * @author: Anton Bondarev
 */

#ifndef UART_DEVICE_H_
#define UART_DEVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include <util/dlist.h>
#include <kernel/irq.h>

#define UART_NAME_MAXLEN 16

#define UART_STATE_OPEN  (1 << 0)
#define UART_CTS_ENABLE  (1 << 1))
#define UART_DCD_ENABLE  (1 << 2))
#define UART_AUTORTS     (1 << 3))
#define UART_AUTOCTS     (1 << 4))
#define UART_AUTOXOFF    (1 << 5))
#define UART_SYNC_FIFO   (1 << 6))

struct uart;
struct uart_desc;
struct tty;

struct uart_params {
	uint32_t baud_rate;
	uint32_t uart_param_flags;
	int n_stop;
	int n_bits;
	bool irq;
};

#define UART_PARAM_FLAGS_PARITY        (0x1 << 0)
#define UART_PARAM_FLAGS_AUTO_CTS      (0x1 << 1)
#define UART_PARAM_FLAGS_AUTO_RTS      (0x1 << 2)
#define UART_PARAM_FLAGS_HARD_FLOW \
	(UART_PARAM_FLAGS_AUTO_CTS | UART_PARAM_FLAGS_AUTO_RTS))

struct uart_ops {
	int (*uart_getc)(struct uart *dev);
	int (*uart_putc)(struct uart *dev, int symbol);
	int (*uart_hasrx)(struct uart *dev);
	int (*uart_setup)(struct uart *dev, const struct uart_params *params);
	int (*uart_irq_en)(struct uart *dev, const struct uart_params *params);
	int (*uart_irq_dis)(struct uart *dev, const struct uart_params *params);
};

struct uart {
	/* declarative */
	const struct uart_ops *uart_ops;
	short irq_num;
	uint32_t base_addr;
	irq_handler_t irq_handler;

	/* management */
	struct dlist_head lnk;
	char dev_name[UART_NAME_MAXLEN];
	int idx;

	/* runtime */
	int state;
	struct uart_params params;
	struct tty *tty;
};


/**
 * @brief Register uart in kernel
 *
 * @param uart Uart descriptor
 * @param uart_params Pointer to uart params. If not null, used to assign
 * 	default params
 *
 * @return uart instance pointer on success
 * @return NULL on error
 */
extern int uart_register(struct uart *uartd,
		const struct uart_params *uart_defparams);

/**
 * @brief Deregister in kernel
 *
 * @param uart
 */
extern void uart_deregister(struct uart *uart);

/**
 * @brief Look uart by name
 *
 * @param name
 *
 * @return uart instance pointer if found
 * @return NULL if not found
 */
extern struct uart *uart_dev_lookup(const char *name);

/**
 * @brief Initialize UART: init tty and assing irq
 *
 * @param uart UART to initilize
 *
 * @return 0 on succes
 * @return error code otherwise
 */
extern int uart_open(struct uart *uart);

/**
 * @brief Deinitialize UART
 *
 * @param uart
 *
 * @return
 */
extern int uart_close(struct uart *uart);

/**
 * @brief Get uart parameters
 *
 * @param uart
 * @param params Pointer to store parameters
 *
 * @return 0 on succes
 */
extern int uart_get_params(struct uart *uart, struct uart_params *params);

/**
 * @brief Set parameters of opened UART. This will perform additional actions
 * to maintain irq parameter and other options.
 *
 * @param uart
 * @param params
 *
 * @return 0 on succes
 */
extern int uart_set_params(struct uart *uart, const struct uart_params *params);

static inline int uart_putc(struct uart *uart, int ch) {
	return uart->uart_ops->uart_putc(uart, ch);
}

static inline int uart_getc(struct uart *uart) {
	return uart->uart_ops->uart_getc(uart);
}

static inline int uart_hasrx(struct uart *uart) {
	return uart->uart_ops->uart_hasrx(uart);
}

#endif /* UART_DEVICE_H_ */

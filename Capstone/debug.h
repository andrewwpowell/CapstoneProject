#ifndef DEBUG_H_
#define DEBUG_H_

// Debug pin
#define DEBUG_BIT               BIT4
#define DEBUG_PORT              P2OUT
#define DEBUG_DDR               P2DIR
#define SET_DEBUG_TO_AN_OUTPUT  DEBUG_DDR |= DEBUG_BIT
#define SET_DEBUG_HIGH          DEBUG_PORT |= DEBUG_BIT
#define SET_DEBUG_LOW           DEBUG_PORT &= ~DEBUG_BIT
#define TOGGLE_DEBUG            DEBUG_PORT ^= DEBUG_BIT

#endif /* DEBUG_H_ */

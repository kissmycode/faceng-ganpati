#ifndef _utilities_h_
#define _utilities_h_

// Debug management
#define DEBUG true

#if DEBUG == true
#pragma warning "Debug-printing is active"
#define DEBUG_PRINT( x, ... ) {char dbgBuf[120]; snprintf_P( dbgBuf, 120, PSTR( x ), ##__VA_ARGS__ ) ; Serial.print( dbgBuf ); }
#else
#define DEBUG_PRINT( x, ... ) ;
#endif

#endif // _utilities_h_
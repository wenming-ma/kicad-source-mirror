// QT_TRANSFORMATION_COMPLETED

#ifndef EDA_DDE_H_
#define EDA_DDE_H_

#include <string>
#include <QTcpSocket>


// TCP/IP ports used by Pcbnew and Eeschema respectively.

/// Pcbnew listens on this port for commands from Eeschema.
#define KICAD_PCB_PORT_SERVICE_NUMBER   4242

/// Eeschema listens on this port for commands from Pcbnew.
#define KICAD_SCH_PORT_SERVICE_NUMBER   4243

/// Scripting window listens for commands for other apps.
#define KICAD_PY_PORT_SERVICE_NUMBER    4244


#define MSG_TO_PCB                      KICAD_PCB_PORT_SERVICE_NUMBER
#define MSG_TO_SCH                      KICAD_SCH_PORT_SERVICE_NUMBER

bool SendCommand( int aPort, const std::string& aMessage );

/// Must be called to clean up the socket thread used by SendCommand.
void SocketCleanup();

#endif    // EDA_DDE_H_

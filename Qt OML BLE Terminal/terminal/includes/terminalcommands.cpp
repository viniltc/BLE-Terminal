#include "terminalcommands.h"

TerminalCommands::TerminalCommands()
{

}

void TerminalCommands::nop()
{
    MCU_CMD_NOP_t cmd;
    cmd.cmdId = MCU_CMD_NOP;
    BLEModule_Tx(&cmd, sizeof(cmd));
}

void TerminalCommands::onmcureset()
{
    MCU_CMD_ON_MCU_RESET_t cmd;
    cmd.cmdId = MCU_CMD_ON_MCU_RESET;
    BLEModule_Tx(&cmd, sizeof(cmd));
}

void TerminalCommands::getnodeid()
{
    MCU_CMD_GET_NODE_ID_t cmd;
    cmd.cmdId = MCU_CMD_GET_NODE_ID;
    BLEModule_Tx(&cmd, sizeof(cmd));
}


void TerminalCommands::fwver()
{
    MCU_CMD_GET_FW_VERSION_t cmd;
    cmd.cmdId = MCU_CMD_GET_FW_VERSION;
    BLEModule_Tx(&cmd, sizeof(cmd));
}

void TerminalCommands::connectble(TerminalArg_t *args)
{

    MCU_CMD_CONNECT_t cmd;
    NodeId_t nodeId = (NodeId_t)args->l;
    cmd.cmdId = MCU_CMD_CONNECT;
    cmd.nodeId[0] = GetArrayByteFromNodeId(0, nodeId);
    cmd.nodeId[1] = GetArrayByteFromNodeId(1, nodeId);
    cmd.nodeId[2] = GetArrayByteFromNodeId(2, nodeId);
    BLEModule_Tx(&cmd, sizeof(cmd));
}

void TerminalCommands::disconnectble(TerminalArg_t *args)
{
    MCU_CMD_DISCONNECT_t cmd;
    NodeId_t nodeId = (NodeId_t)args->l;
    cmd.cmdId = MCU_CMD_DISCONNECT;
    cmd.nodeId[0] = GetArrayByteFromNodeId(0, nodeId);
    cmd.nodeId[1] = GetArrayByteFromNodeId(1, nodeId);
    cmd.nodeId[2] = GetArrayByteFromNodeId(2, nodeId);
    BLEModule_Tx(&cmd, sizeof(cmd));
}

void TerminalCommands::txpayload(TerminalArg_t *args)
{
    uint8_t txBuffer[260];
    MCU_CMD_TX_PAYLOAD_t *cmd = (MCU_CMD_TX_PAYLOAD_t *)txBuffer;

    cmd->cmdId = MCU_CMD_TX_PAYLOAD;
    NodeId_t nodeId = (NodeId_t)args[0].l;
    cmd->destNodeId[0] = GetArrayByteFromNodeId(0, nodeId);
    cmd->destNodeId[1] = GetArrayByteFromNodeId(1, nodeId);
    cmd->destNodeId[2] = GetArrayByteFromNodeId(2, nodeId);
    cmd->ack = 0;

    size_t payloadLen = strlen(args[1].s);
    cmd->payloadLen = (uint8_t)payloadLen;
    (void)memcpy(&cmd->payloadLen + 1u, args[1].s, payloadLen);

    BLEModule_Tx(cmd, 6u + payloadLen);
}

void TerminalCommands::txpayloadack(TerminalArg_t *args)
{
    uint8_t txBuffer[260];
    MCU_CMD_TX_PAYLOAD_t *cmd = (MCU_CMD_TX_PAYLOAD_t *)txBuffer;

    cmd->cmdId = MCU_CMD_TX_PAYLOAD;
    NodeId_t nodeId = (NodeId_t)args[0].l;
    cmd->destNodeId[0] = GetArrayByteFromNodeId(0, nodeId);
    cmd->destNodeId[1] = GetArrayByteFromNodeId(1, nodeId);
    cmd->destNodeId[2] = GetArrayByteFromNodeId(2, nodeId);
    cmd->ack = 1u;

    size_t payloadLen = strlen(args[1].s);
    cmd->payloadLen = (uint8_t)payloadLen;
    (void)memcpy(&cmd->payloadLen + 1u, args[1].s, payloadLen);

    BLEModule_Tx(cmd, 6u + payloadLen);
}


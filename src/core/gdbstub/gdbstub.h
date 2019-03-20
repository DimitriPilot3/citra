// Copyright 2013 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

// Originally written by Sven Peter <sven@fail0verflow.com> for anergistic.

#pragma once

#include "common/common_types.h"
#include "core/hle/kernel/thread.h"

namespace GDBStub {

/// Breakpoint Method
enum class BreakpointType {
    None,    ///< None
    Execute, ///< Execution Breakpoint
    Read,    ///< Read Breakpoint
    Write,   ///< Write Breakpoint
    Access   ///< Access (R/W) Breakpoint
};

struct BreakpointAddress {
    VAddr address;
    BreakpointType type;
};

/**
 * Set the port the gdbstub should use to listen for connections.
 *
 * @param port Port to listen for connection
 */
void SetServerPort(u16 port);

/**
 * Starts or stops the server if possible.
 *
 * @param status Set the server to enabled or disabled.
 */
void ToggleServer(bool status);

/// Start the gdbstub server.
void Init();

/// Stop gdbstub server.
void Shutdown();

/// Checks if the gdbstub server is enabled.
bool IsServerEnabled();

/// Returns true if there is an active socket connection.
bool IsConnected();

/**
 * Signal to the gdbstub server that it should halt CPU execution.
 */
void Break();

/// Determine if there was a memory breakpoint.
bool IsMemoryBreak();

/// Read and handle packet from gdb client.
void HandlePacket();

/**
 * Get the nearest breakpoint of the specified type at the given address.
 *
 * @param addr Address to search from.
 * @param type Type of breakpoint.
 */
BreakpointAddress GetNextBreakpointFromAddress(VAddr addr, GDBStub::BreakpointType type);

/**
 * Check if any breakpoints of the given type are watching the given memory block.
 * If one such breakpoint is found, this function returns its start address.
 *
 * @param addr Address of the memory block.
 * @param len  How long/wide the memory block is, in bytes.
 * @param type Type of breakpoint to search for.
 */
std::optional<VAddr> CheckBreakpoint(VAddr addr, u32 len, GDBStub::BreakpointType type);

// If set to true, the CPU will halt at the beginning of the next CPU loop.
bool GetCpuHaltFlag();

// If set to true and the CPU is halted, the CPU will step one instruction.
bool GetCpuStepFlag();

/**
 * When set to true, the CPU will step one instruction when the CPU is halted next.
 *
 * @param is_step
 */
void SetCpuStepFlag(bool is_step);

/**
 * Send trap signal from thread back to the gdbstub server.
 *
 * @param thread Sending thread.
 * @param trap Trap no.
 */
void SendTrap(Kernel::Thread* thread, int trap);

/**
 * Signal to the gdbstub server that the CPU hit a watchpoint at a given data address.
 *
 * The gdb client attempts to find the culprit watchpoint based on the reported address.
 * It must fall within a watchpoint's address interval, as defined by its address + size.
 * Therefore, any address "within range is sufficient" - it doesn't have to be exact.
 *
 * While this client-side range check is a reasonable default for new stub servers, it's
 * limited by the remote protocol: we can't specify how big the reported data region is.
 *
 * For example, a watchpoint targetting *(char *)0x1237 could be hit by Read32(0x1234)
 * server-side. However, for the client to register a hit, the reported address must
 * EXACTLY be 0x1237 (watchpoint address) and not 0x1234 (data operation address)!
 *
 * @param address Hit address.
 */
void OnWatchpointHit(VAddr address);
} // namespace GDBStub

# RFC: MyTeams Client-Server Communication Protocol & Architecture

**Document Status:** Draft  
**Version:** 1.0  
**Date:** April 2026  
**Author:** Development Team  
**Type:** Technical Specification  

---

## 1. Introduction

This RFC documents the complete architecture, communication protocol, and implementation details of the MyTeams system—a client-server application for team collaboration featuring users, teams, channels, threads, and private messaging capabilities.

### 1.1 Purpose

This document serves as a comprehensive reference for:
- Software architects implementing protocol extensions
- Developers maintaining and debugging the system
- System integrators building complementary tools
- Future maintainers evolving the codebase

### 1.2 Scope

This RFC covers:
- Client-server TCP/IP communication protocol
- Message format and status codes
- Command specification and execution flow
- Server architecture and data structures
- Persistence layer and file formats
- User context management and notifications

---

## 2. Executive Summary

MyTeams is a team collaboration platform implementing a **command-response** architecture over TCP/IP sockets. The system features:

- **Hierarchical structure:** Teams → Channels → Threads → Comments
- **Private messaging** between users
- **Session management** with authentication and context switching
- **Persistence** via line-delimited JSON files
- **Real-time notifications** to all connected clients
- **UUID-based identification** for all entities (RFC 4122 format)

---

## 3. System Architecture

### 3.1 High-Level Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     MyTeams System                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐              ┌──────────────┐             │
│  │   Client     │◄────TCP─────►│   Server     │             │
│  │ (CLI-based)  │   (5 min     │  (Engine)    │             │
│  └──────────────┘   timeout)   └──────────────┘             │
│                                        │                    │
│                         ┌──────────────┴──────────────┐     │
│                         │                             │     │
│                    ┌────▼─────┐             ┌────────▼───┐  │
│                    │  Engine  │             │ Persistence│  │
│                    │ (Command │             │   Layer    │  │
│                    │ Executor)│             │ (AEL files)│  │
│                    └──────────┘             └────────────┘  │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 Component Overview

#### 3.2.1 Client (myteams_cli)
- **Language:** C++
- **Socket Type:** TCP client
- **Responsibilities:**
  - Parse user commands from CLI
  - Serialize commands to protocol format
  - Connect to server (port provided at runtime)
  - Parse and display server responses
  - Maintain local context (current team/channel/thread)
  - Handle async notifications from server

#### 3.2.2 Server (myteams_server)
- **Language:** C++
- **Socket Type:** TCP server with multiplexed I/O (poll)
- **Key Classes:**
  - `Server`: Entry point, runs event loop
  - `Engine`: Command processing and business logic
  - `Socket`: TCP socket abstraction
  - `PollFD`: Event multiplexer (poll-based)
  - `User`: Represents authenticated/anonymous sessions
  - `Team/Channel/Thread/Comment`: Data models

#### 3.2.3 Data Models
```
User
├── username (string, max 32 chars)
├── uuid (UUID v4)
├── authenticated (boolean)
├── file_descriptor (int)
├── current_team (UUID or empty)
├── current_channel (UUID or empty)
└── current_thread (UUID or empty)

Team
├── name (string, max 32 chars)
├── uuid (UUID v4)
├── description (string, max 255 chars)
├── timestamp (unix time_t)
└── users_uuid[] (array of user UUIDs)

Channel
├── name (string, max 32 chars)
├── uuid (UUID v4)
├── description (string, max 255 chars)
├── team_uuid (UUID v4)
└── timestamp (unix time_t)

Thread
├── name (string, max 32 chars)
├── uuid (UUID v4)
├── description (string, max 512 chars)
├── channel_uuid (UUID v4)
├── author_uuid (UUID v4)
└── timestamp (unix time_t)

Comment
├── uuid (UUID v4)
├── body (string, max 512 chars)
├── thread_uuid (UUID v4)
├── author_uuid (UUID v4)
└── timestamp (unix time_t)

Message (Private)
├── sender_uuid (UUID v4)
├── receiver_uuid (UUID v4)
├── body (string, max 512 chars)
└── timestamp (unix time_t)
```

---

## 4. Communication Protocol

### 4.1 Transport Layer

- **Protocol:** TCP/IP (IPv4)
- **Port:** Configurable, passed to server at startup
- **Default Timeout:** 5 minutes (client-side)
- **Connection Model:** Long-lived persistent connections
- **Multiplexing:** Server uses poll() for handling multiple clients

### 4.2 Message Format

#### 4.2.1 General Message Structure

All messages consist of:
1. **Message Code** (numeric identifier)
2. **Parameters** (space/quote-separated)
3. **Line Terminator** (LF `\n` or CRLF `\r\n`)

**Note:** The server accepts both line termination formats:
- LF only: `\n`
- CRLF: `\r\n`

#### 4.2.2 Message Format Patterns

**Format 1: Code Only**
```
<code>\r\n
```
Example: `4005\r\n`

**Format 2: Code + Numeric Parameter**
```
<code> <param>\r\n
```
Example: `2200 550e8400-e29b-41d4-a716-446655440000 "john" 1\r\n`

**Format 3: Code + Quoted String**
```
<code> "<param>"\r\n
```
Example: `220 550e8400-e29b-41d4-a716-446655440000 "alice"\r\n`

**Format 4: Mixed Parameters**
```
<code> <uuid> "<string>" <numeric> "<string>"\r\n
```
Example: `2302 thread-uuid author-uuid 1650000000 "Thread Title" "Thread body content"`

### 4.3 User State Management

#### 4.3.1 User Types

**Anonymous User**
- Not yet authenticated
- Can only execute LOGIN/HELP commands
- FD tracked for timeout management

**Authenticated User**
- Successfully logged in with username
- Can execute all commands
- Maintains local context (team/channel/thread)
- Can receive notifications

**Offline User**
- Previously logged in but disconnected
- Stored on server to preserve user data
- Receives private messages even while offline

#### 4.3.2 User Context

The user's current navigation location:

```cpp
struct UserContext {
    string current_team;      // UUID, empty = no team context
    string current_channel;   // UUID, empty = no channel context  
    string current_thread;    // UUID, empty = no thread context
};
```

Commands may require specific context levels:
- `IFU`: No context required
- `IFT`: Team context required
- `IFC`: Channel context required
- `IFH`: Thread context required

### 4.4 Command Format

#### 4.4.1 Client-to-Server Commands

Commands are sent as single-line strings terminated by CRLF:

```
<COMMAND> [args...]\r\n
```

Examples:
```
LOGIN alice\r\n
CREATE_TEAM "Development" "Team for development tasks"\r\n
USE team-uuid\r\n
SEND user-uuid "Hello, world!"\r\n
```

#### 4.4.2 Command Execution Flow

```
Client                          Server
  │                               │
  ├──────── Command ────────────►│
  │                          ┌────▼────┐
  │                          │ Parsing │
  │                          └────┬────┘
  │                          ┌────▼──────────┐
  │                          │ Authentication│
  │                          │ Check         │
  │                          └────┬──────────┘
  │                          ┌────▼──────────┐
  │                          │ Command       │
  │                          │ Validation    │
  │                          └────┬──────────┘
  │                          ┌────▼──────────┐
  │                          │ Execution     │
  │                          │ (Update state)│
  │                          └────┬──────────┘
  │                          ┌────▼──────────┐
  │                          │ Notification  │
  │                          │ Broadcast     │
  │                          └────┬──────────┘
  │                               │
  │◄────── Response ──────────────┤
  │                               │
  │◄──── Notifications ───────────┤
  │                               │
```

---

## 5. Command Reference

### 5.1 Authentication Commands

#### LGI (LOGIN)
**Usage:** `LOGIN <username>`  
**Authentication Required:** No  
**Context Required:** None  
**Response:** `220 <user_uuid> "<username>"` (Success) or `230` (User exists with active session)  

**Behavior:**
- Creates new user if username doesn't exist
- Reuses offline user with matching username if available
- Rejects if user already has active authenticated session
- Broadcasts login notification to all authenticated users

#### LGO (LOGOUT)
**Usage:** `LOGOUT`  
**Authentication Required:** Yes  
**Context Required:** None  
**Response:** `221 <user_uuid> "<username>"`  

**Behavior:**
- Moves user to offline state
- Clears user context
- Broadcasts logout notification

### 5.2 Navigation Commands

#### USE (Context Switching)
**Usage:** `USE [team_uuid] [channel_uuid] [thread_uuid]`  
**Authentication Required:** Yes  
**Context Required:** Varies  

**Behaviors:**
- `USE` (no args): Clear all context, response `999`
- `USE team_uuid`: Set team context
- `USE team_uuid channel_uuid`: Set team and channel context
- `USE team_uuid channel_uuid thread_uuid`: Set all contexts
- Invalid: Response `410` or `411` (context validation error)

### 5.3 Team Commands

#### CRT (CREATE_TEAM)
**Usage:** `CREATE_TEAM "<name>" "<description>"`  
**Authentication Required:** Yes  
**Response:** `2300 <team_uuid> "<name>" "<description>"`  

**Constraints:**
- Name: 1-32 chars
- Description: 1-255 chars
- User auto-subscribed to new team

#### SUB (SUBSCRIBE_TEAM)
**Usage:** `SUBSCRIBE_TEAM <team_uuid>`  
**Authentication Required:** Yes  
**Response:** `2400 <user_uuid> <team_uuid>`  

**Errors:**
- `4001`: Unknown team
- `410`: Already subscribed

#### UNS (UNSUBSCRIBE_TEAM)
**Usage:** `UNSUBSCRIBE_TEAM <team_uuid>`  
**Authentication Required:** Yes  
**Response:** `2401 <user_uuid> <team_uuid>`  

**Errors:**
- `4001`: Unknown team
- `411`: Not subscribed

#### LST (LIST)
**Usage:** `LIST [type] [target_uuid]`  
**Authentication Required:** Yes  

**Subtypes:**
- `LST USS`: List all users (online/offline)
- `LST SBD [team_uuid]`: List subscribed teams or team's users
- `LST LSH`: List threads in current channel
- `LST LRC [thread_uuid]`: List comments/replies in thread
- `LST MSG <user_uuid>`: List private messages with user

**Response Format:** Multiple lines, each starting with message code (21xx range)

### 5.4 Channel Commands

#### CRC (CREATE_CHANNEL)
**Usage:** `CREATE_CHANNEL "<name>" "<description>"`  
**Authentication Required:** Yes  
**Context Required:** Team  
**Response:** `2301 <channel_uuid> "<name>" "<description>"`  

#### CRH (CREATE_THREAD)
**Usage:** `CREATE_THREAD "<title>" "<body>"`  
**Authentication Required:** Yes  
**Context Required:** Channel  
**Response:** `2302 <thread_uuid> <author_uuid> <timestamp> "<title>" "<body>"`  

#### CRR (CREATE_REPLY)
**Usage:** `CREATE_REPLY "<body>"`  
**Authentication Required:** Yes  
**Context Required:** Thread  
**Response:** `2303 <thread_uuid> <author_uuid> <timestamp> "<body>"`  

### 5.5 Messaging Commands

#### SND (SEND)
**Usage:** `SEND <recipient_uuid> "<message>"`  
**Authentication Required:** Yes  
**Response:** `1004 <sender_uuid> <recipient_uuid> <timestamp> "<body>"`  

**Behavior:**
- Stores message in persistent storage
- Notifies recipient if online (message code `2000`)
- Recipient can retrieve with `MSG` command

#### MSG (RETRIEVE_MESSAGES)
**Usage:** `MESSAGES <user_uuid>`  
**Authentication Required:** Yes  
**Response:** Multiple `2105` messages or empty

### 5.6 Information Commands

#### USR (USER_INFO)
**Usage:** `USER <username_or_uuid>`  
**Authentication Required:** Yes  
**Response:** `2200 <user_uuid> "<username>" <status>`  
- Status: `1` (online) or `0` (offline)

#### INF (INFO - Context-aware)
**Usage:** `INFO [type]`  
**Authentication Required:** Yes  

**Subtypes:**
- `INF IFU`: Get current user info
- `INF IFT`: Get current team info (requires team context)
- `INF IFC`: Get current channel info (requires channel context)
- `INF IFH`: Get current thread info (requires thread context)

**Responses:**
- `2200`: User info
- `2201`: Team info
- `2202`: Channel info
- `2203`: Thread info

#### HLP (HELP)
**Usage:** `HELP`  
**Authentication Required:** No  
**Response:** `320 "<help_text>"`  

---

## 6. Status Code Reference

### 6.1 2xx Series: Authentication & Session Events

| Code | Format | Meaning |
|------|--------|---------|
| 220 | `220 <uuid> "<username>"` | User logged in |
| 221 | `221 <uuid> "<username>"` | User logged out |
| 230 | `230` | User already exists (duplicate login) |
| 999 | `999` | Context cleared (USE with no args) |

### 6.2 2x00 Series: Notifications (Server→Client Broadcast)

| Code | Format | Meaning |
|------|--------|---------|
| 2000 | `2000 <sender_uuid> "<body>"` | Private message received |
| 2001 | `2001 <team_uuid> <thread_uuid> <user_uuid> "<body>"` | Reply posted in thread |
| 2002 | `2002 <team_uuid> "<name>" "<desc>"` | New team created |
| 2003 | `2003 <channel_uuid> "<name>" "<desc>"` | New channel created in team |
| 2004 | `2004 <thread_uuid> <user_uuid> <timestamp> "<title>" "<body>"` | New thread created in channel |

### 6.3 21xx Series: List Items

| Code | Format | Meaning |
|------|--------|---------|
| 2100 | `2100 <uuid> "<username>" <status>` | User list item (0=offline, 1=online) |
| 2101 | `2101 <uuid> "<name>" "<desc>"` | Team list item |
| 2102 | `2102 <uuid> "<name>" "<desc>"` | Channel list item |
| 2103 | `2103 <uuid> <author> <ts> "<name>" "<body>"` | Thread list item |
| 2104 | `2104 <uuid> <author> <ts> "<body>"` | Comment/reply list item |
| 2105 | `2105 <sender_uuid> <timestamp> "<body>"` | Private message list item |

### 6.4 22xx Series: Single Item Info

| Code | Format | Meaning |
|------|--------|---------|
| 2200 | `2200 <uuid> "<username>" <status>` | User info |
| 2201 | `2201 <uuid> "<name>" "<desc>"` | Team info |
| 2202 | `2202 <uuid> "<name>" "<desc>"` | Channel info |
| 2203 | `2203 <uuid> <author> <ts> "<name>" "<body>"` | Thread info |

### 6.5 23xx Series: Creation Success

| Code | Format | Meaning |
|------|--------|---------|
| 2300 | `2300 <uuid> "<name>" "<desc>"` | Team created |
| 2301 | `2301 <uuid> "<name>" "<desc>"` | Channel created |
| 2302 | `2302 <uuid> <author> <ts> "<title>" "<body>"` | Thread created |
| 2303 | `2303 <uuid> <author> <ts> "<body>"` | Reply created |

### 6.6 24xx Series: Subscription Management

| Code | Format | Meaning |
|------|--------|---------|
| 2400 | `2400 <user_uuid> <team_uuid>` | Subscribed to team |
| 2401 | `2401 <user_uuid> <team_uuid>` | Unsubscribed from team |

### 6.7 3xx Series: Server Information

| Code | Format | Meaning |
|------|--------|---------|
| 320 | `320 "<message>"` | Server info message |

### 6.8 4xx Series: Client Errors

| Code | Format | Meaning |
|------|--------|---------|
| 400 | `400: <message>` | Generic client error |
| 410 | `410: <message>` | Invalid command/arguments |
| 411 | `411: <message>` | Missing context (team/channel/thread) |
| 412 | `412: <message>` | Invalid context value |
| 413 | `413: <message>` | Generic validation error |
| 500 | `500: <message>` | Server internal error |
| 530 | `530: <message>` | Permission denied |

### 6.9 40xx Series: Resource Not Found

| Code | Format | Meaning |
|------|--------|---------|
| 4001 | `4001 <uuid>` | Unknown team |
| 4002 | `4002 <uuid>` | Unknown channel |
| 4003 | `4003 <uuid>` | Unknown thread |
| 4004 | `4004 <uuid>` | Unknown user |
| 4005 | `4005` | User not authenticated/authorized |

### 6.10 Other Codes

| Code | Format | Meaning |
|------|--------|---------|
| 1004 | `1004 <sender> <recipient> <ts> "<body>"` | Message sent (confirmation) |

---

## 7. Message Flow Examples

### 7.1 Example 1: User Login & Team Creation

```
Timeline:
  T1: User A connects (anonymous state)
  T2: User B connects (anonymous state)
  T3: User A logs in
  T4: User A creates team
  T5: User B logs in

Flow:

T1: UserA: socket connect → Server accepts on listening socket

T3: UserA → Server: "LOGIN alice\r\n"
    Server: UserA → "220 <uuid-a> \"alice\"\r\n"
    Server: All authenticated users ← "220 <uuid-a> \"alice\"\r\n" (notification)
    Server: Move UserA from anonymous to authenticated list

T4: UserA → Server: "CREATE_TEAM \"Engineering\" \"Main eng team\"\r\n"
    Server: Creates new team with UUID
    Server: UserA ← "2300 <team-uuid> \"Engineering\" \"Main eng team\"\r\n"
    Server: All team subscribers ← "2002 <team-uuid> \"Engineering\" \"Main eng team\"\r\n"

T5: UserB → Server: "LOGIN bob\r\n"
    Server: UserB ← "220 <uuid-b> \"bob\"\r\n"
    Server: All authenticated ← "220 <uuid-b> \"bob\"\r\n" (notification)
```

### 7.2 Example 2: Channel & Thread Creation with Reply

```
T1: UserA: "USE <team-uuid>\r\n"
    Server: UserA context = {team: team-uuid, channel: "", thread: ""}
    Server: UserA ← "999   \r\n"

T2: UserA: "CREATE_CHANNEL \"General\" \"General discussion\"\r\n"
    Server: Creates channel in team context
    Server: UserA ← "2301 <ch-uuid> \"General\" \"General discussion\"\r\n"
    Server: Team subscribers ← "2003 <ch-uuid> \"General\" \"General discussion\"\r\n"

T3: UserA: "USE <team-uuid> <ch-uuid>\r\n"
    Server: UserA context = {team: team-uuid, channel: ch-uuid, thread: ""}
    Server: UserA ← "999   \r\n"

T4: UserA: "CREATE_THREAD \"Q3 Planning\" \"Let's discuss Q3 goals\"\r\n"
    Server: Creates thread with author=UserA, timestamp=now
    Server: UserA ← "2302 <th-uuid> <uuid-a> 1650000000 \"Q3 Planning\" \"Let's discuss Q3 goals\"\r\n"
    Server: Team subscribers ← "2004 <th-uuid> <uuid-a> 1650000000 \"Q3 Planning\" \"Let's discuss Q3 goals\"\r\n"

T5: UserA: "USE <team-uuid> <ch-uuid> <th-uuid>\r\n"
    Server: UserA context = {team: ..., channel: ..., thread: th-uuid}
    Server: UserA ← "999   \r\n"

T6: UserA: "CREATE_REPLY \"I agree, we need clear metrics\"\r\n"
    Server: Appends reply to thread
    Server: UserA ← "2303 <th-uuid> <uuid-a> 1650000001 \"I agree, we need clear metrics\"\r\n"
    Server: Team subscribers ← "2001 <team-uuid> <th-uuid> <uuid-a> \"I agree, we need clear metrics\"\r\n"
```

### 7.3 Example 3: Private Messaging

```
T1: UserA: "SEND <uuid-b> \"Hey Bob, check the design docs\"\r\n"
    Server: Stores message in persistent storage
    Server: UserA ← "1004 <uuid-a> <uuid-b> 1650000100 \"Hey Bob, check the design docs\"\r\n"
    
    [If UserB is online]
    Server: UserB ← "2000 <uuid-a> \"Hey Bob, check the design docs\"\r\n" (notification)
    
    [If UserB is offline]
    Server: Message stored, will be retrieved when UserB requests MSG

T2: UserB: "MESSAGES <uuid-a>\r\n"
    Server: Retrieves all messages with UserA
    Server: UserB ← "2105 <uuid-a> 1650000100 \"Hey Bob, check the design docs\"\r\n"
```

### 7.4 Example 4: Subscription Management

```
T1: UserA (subscribed to Team1): "SUBSCRIBED\r\n"
    Server: Returns teams UserA is subscribed to
    Server: UserA ← "2101 <team1-uuid> \"Team1\" \"...\"\r\n"

T2: UserA: "SUBSCRIBE_TEAM <team2-uuid>\r\n"
    Server: Adds UserA to Team2
    Server: UserA ← "2400 <uuid-a> <team2-uuid>\r\n"
    Server: Team2 subscribers ← "2002 ...\r\n" (if new team, else no notification)

T3: UserA: "UNSUBSCRIBE_TEAM <team2-uuid>\r\n"
    Server: Removes UserA from Team2
    Server: UserA ← "2401 <uuid-a> <team2-uuid>\r\n"
```

---

## 8. Persistence Layer

### 8.1 File Format Overview

All persistence files use **line-delimited JSON** format:
- One JSON object per line
- Files located in server's working directory
- Files read entirely at startup, written entirely at shutdown
- No array wrapper (streaming format)

### 8.2 File Specifications

#### users.AEL
```json
{"name": "alice", "userUuid": "550e8400-e29b-41d4-a716-446655440000"}
{"name": "bob", "userUuid": "550e8400-e29b-41d4-a716-446655440001"}
```

#### teams.AEL
```json
{"name": "Engineering", "teamUuid": "...", "description": "Main eng team", "usersUuid": ["uuid1", "uuid2"]}
```

#### channels.AEL
```json
{"name": "General", "channelUuid": "...", "description": "...", "teamUuid": "..."}
```

#### threads.AEL
```json
{"name": "Q3 Planning", "threadUuid": "...", "description": "...", "channelUuid": "...", "authorUuid": "..."}
```

#### comments.AEL
```json
{"body": "Reply text", "commentUuid": "...", "threadUuid": "...", "authorUuid": "..."}
```

#### messages.AEL
```json
{"senderUuid": "...", "receiverUuid": "...", "body": "Message text"}
```

### 8.3 Persistence Strategy

**Load Phase (Server Startup):**
1. Read all `.AEL` files
2. Reconstruct in-memory data structures
3. Restore relationships (teams→channels→threads→comments)

**Save Phase (Server Shutdown):**
1. Serialize all data structures to JSON
2. Write entire contents to respective `.AEL` files
3. Atomically complete or fail (no partial saves)

**Data Consistency:**
- No transactions mid-run (entire operation succeeds/fails)
- All changes kept in memory during runtime
- Changes persist only after clean shutdown

---

## 9. Server Implementation Details

### 9.1 Event Loop & Multiplexing

The server uses **poll()** for I/O multiplexing:

```cpp
Server::run() {
    while (server_loop) {
        int poll_result = poll(&fds[0], fds.size(), TIMEOUT);
        
        if (poll_result > 0) {
            for (each pollfd) {
                if (events & POLLIN) {
                    // Handle readable socket
                    if (is_control_socket) {
                        accept_new_connection();
                    } else {
                        read_command();
                    }
                }
            }
        }
    }
}
```

### 9.2 Command Processing Pipeline

```
Raw Input (with CRLF)
    ↓
[Split by CRLF]
    ↓
[Tokenize into command + args]
    ↓
[Authentication check]
    ↓
[Context validation]
    ↓
[Execute via Engine::exec()]
    ↓
[Generate response(s)]
    ↓
[Broadcast notifications]
    ↓
[Send to client(s)]
```

### 9.3 Error Handling Strategy

**Error Hierarchy:**
```cpp
AServerError (base)
├── Minor: Recoverable errors, continue execution
├── Major: Significant errors, may need cleanup
└── Critical: Unrecoverable, shutdown required
```

**Error Response:**
- Authentication error → M4005
- Resource not found → M40xx
- Invalid arguments → M410
- Missing context → M411
- Server error → M500

---

## 10. Client Implementation Details

### 10.1 Command Processing

Client maintains a map of status codes to response handlers:

```cpp
{
    "220": [](const string& resp) { /* LOGIN response */ },
    "2300": [](const string& resp) { /* TEAM_CREATED */ },
    "2000": [](const string& resp) { Receiver::display(resp); }, // Notification
    "4001": [](const string& resp) { /* UNKNOWN_TEAM error */ },
    // ... 30+ handlers
}
```

### 10.2 Response Parsing

Responses may contain multiple lines (for batch operations):

```
LIST USS response:
2100 uuid1 "alice" 1
2100 uuid2 "bob" 0
2100 uuid3 "charlie" 1

MESSAGES response:
2105 uuid1 1650000100 "first message"
2105 uuid1 1650000101 "second message"
```

Each line is parsed by its leading code and dispatched to the handler.

---

## 11. References & Related Documents

- **PERSISTENCE_FORMAT.md:** Detailed persistence file specification
- **RFC 4122:** UUID Standard (https://tools.ietf.org/html/rfc4122)
- **POSIX poll():** https://man7.org/linux/man-pages/man2/poll.2.html
- **TCP/IP Protocol Suite:** TCP Standard (RFC 793)

---

## 12. Appendix: Command Quick Reference

| Command | Args | Auth | Context | Response |
|---------|------|------|---------|----------|
| LOGIN | `<username>` | No | - | 220/230 |
| LOGOUT | - | Yes | - | 221 |
| CREATE_TEAM | `"name" "desc"` | Yes | - | 2300 |
| CREATE_CHANNEL | `"name" "desc"` | Yes | Team | 2301 |
| CREATE_THREAD | `"title" "body"` | Yes | Channel | 2302 |
| CREATE_REPLY | `"body"` | Yes | Thread | 2303 |
| SUBSCRIBE_TEAM | `<team_uuid>` | Yes | - | 2400 |
| UNSUBSCRIBE_TEAM | `<team_uuid>` | Yes | - | 2401 |
| SEND | `<user_uuid> "msg"` | Yes | - | 1004 |
| USE | `[t] [c] [th]` | Yes | - | 999/4xx |
| LIST | `[type] [target]` | Yes | - | 21xx |
| USER | `<username>` | Yes | - | 2200 |
| INFO | `[type]` | Yes | Varies | 22xx |
| HELP | - | No | - | 320 |

---

## Document History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | Apr 2026 | Dev Team | Initial RFC draft |

---

**End of RFC**

---

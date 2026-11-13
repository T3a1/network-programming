# MyTeams Persistence Files Format

This document describes the format of all persistence files used by the MyTeams server.

## File Locations
All files are stored in the server's working directory:
- `./users.AEL` - User accounts
- `./teams.AEL` - Teams
- `./channels.AEL` - Channels
- `./threads.AEL` - Threads/Topics
- `./comments.AEL` - Comments/Replies
- `./messages.AEL` - Private messages

## File Formats

### users.AEL
JSON-like format with one user per object. Empty file to start.

**Format:**
```json
{
	"name": "username",
	"userUuid": "550e8400-e29b-41d4-a716-446655440000"
}
```

**Fields:**
- `name`: Username (string)
- `userUuid`: Unique user identifier (UUID)

---

### teams.AEL
JSON-like format with one team per object. Empty file to start.

**Format:**
```json
{
	"name": "team_name",
	"teamUuid": "550e8400-e29b-41d4-a716-446655440001",
	"description": "team description",
	"usersUuid": ["uuid1", "uuid2", "uuid3"]
}
```

**Fields:**
- `name`: Team name (string)
- `teamUuid`: Unique team identifier (UUID)
- `description`: Team description (string)
- `usersUuid`: Array of user UUIDs subscribed to this team

---

### channels.AEL
JSON-like format with one channel per object. Empty file to start.

**Format:**
```json
{
	"name": "channel_name",
	"channelUuid": "550e8400-e29b-41d4-a716-446655440002",
	"description": "channel description",
	"teamUuid": "550e8400-e29b-41d4-a716-446655440001"
}
```

**Fields:**
- `name`: Channel name (string)
- `channelUuid`: Unique channel identifier (UUID)
- `description`: Channel description (string)
- `teamUuid`: UUID of the parent team

---

### threads.AEL
JSON-like format with one thread per object. Empty file to start.

**Format:**
```json
{
	"name": "thread_title",
	"threadUuid": "550e8400-e29b-41d4-a716-446655440003",
	"description": "thread body/content",
	"channelUuid": "550e8400-e29b-41d4-a716-446655440002"
}
```

**Fields:**
- `name`: Thread title (string)
- `threadUuid`: Unique thread identifier (UUID)
- `description`: Thread body/content (string)
- `channelUuid`: UUID of the parent channel

---

### comments.AEL
JSON-like format with one comment per object. Empty file to start.

**Format:**
```json
{
	"body": "comment_content",
	"commentUuid": "550e8400-e29b-41d4-a716-446655440004",
	"threadUuid": "550e8400-e29b-41d4-a716-446655440003",
	"authorUuid": "550e8400-e29b-41d4-a716-446655440000"
}
```

**Fields:**
- `body`: Comment/reply text (string)
- `commentUuid`: Unique comment identifier (UUID)
- `threadUuid`: UUID of the parent thread
- `authorUuid`: UUID of the user who posted the comment

---

### messages.AEL
JSON-like format with one private message per object. Empty file to start.

**Format:**
```json
{
	"senderUuid": "550e8400-e29b-41d4-a716-446655440000",
	"receiverUuid": "550e8400-e29b-41d4-a716-446655440001",
	"body": "message content"
}
```

**Fields:**
- `senderUuid`: UUID of the message sender
- `receiverUuid`: UUID of the message recipient
- `body`: Message text (string)

---

## Notes

- All UUIDs follow RFC 4122 format (e.g., `550e8400-e29b-41d4-a716-446655440000`)
- Each JSON object is on its own line (no array wrapper)
- Files start empty and are populated as users interact with the server
- Files are read entirely on server startup (load) and written entirely on server shutdown (save)
- Special characters in strings should be escaped as needed for JSON parsing

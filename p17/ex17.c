#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATA 512
#define MAX_ROWS 100

struct Address {
  int id;
  int set;
  char *name;
  char *email;
  int data;
};

struct Database {
  struct Address *rows;
  int cnt;
};

struct Connection {
  FILE *file;
  struct Database *db;
};

struct Connection *conn;

void Database_close(struct Connection *conn);

void die(struct Connection *conn, const char *message) {
  if (errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  Database_close(conn);

  exit(1);
}

void Address_print(struct Address *addr) {
  printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load(struct Connection *conn) {
  int rc = fread(&conn->db->cnt, sizeof(int), 1, conn->file);
  if (rc != 1)
    die(conn, "Failed to load database cnt.");

  for (int i = 0; i < conn->db->cnt; i++) {
    struct Address *addr = &conn->db->rows[i];
    rc = fread(&addr->id, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to load id.");
    rc = fread(&addr->set, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to load set.");
    rc = fread(&addr->data, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to load data.");

    rc = fread(addr->name, sizeof(char), addr->data, conn->file);
    if (rc != addr->data)
      die(conn, "Failed to load name.");
    rc = fread(addr->email, sizeof(char), addr->data, conn->file);
    if (rc != addr->data)
      die(conn, "Failed to load email.");
  }
}

struct Connection *Database_open(int data, int rows, const char *filename,
                                 char mode) {
  struct Connection *conn = malloc(sizeof(struct Connection));
  if (!conn)
    die(conn, "Memory error");

  conn->db = malloc(sizeof(struct Database));
  if (!conn->db)
    die(conn, "Memory error");
  conn->db->cnt = rows;
  conn->db->rows = malloc(conn->db->cnt * sizeof(struct Address));
  if (!conn->db->rows)
    die(conn, "Memory error");

  for (int i = 0; i < rows; i++) {
    conn->db->rows[i].data = data;
    conn->db->rows[i].email = malloc(sizeof(char) * data);
    memset(conn->db->rows[i].email, 0, data);
    if (!conn->db->rows[i].email)
      die(conn, "Memory error");
    conn->db->rows[i].name = malloc(sizeof(char) * data);
    memset(conn->db->rows[i].name, 0, data);
    if (!conn->db->rows[i].name)
      die(conn, "Memory error");
  }

  if (mode == 'c') {
    conn->file = fopen(filename, "w");
  } else {
    conn->file = fopen(filename, "r+");

    if (conn->file) {
      Database_load(conn);
    }
  }

  if (!conn->file)
    die(conn, "Failed to open the file");

  return conn;
}

void Database_close(struct Connection *conn) {
  if (conn) {
    if (conn->file)
      fclose(conn->file);
    if (conn->db) {
      if (conn->db->rows) {
        for (int i = 0; i < conn->db->cnt; i++) {
          if (conn->db->rows[i].email)
            free(conn->db->rows[i].email);
          if (conn->db->rows[i].name)
            free(conn->db->rows[i].name);
        }
        free(conn->db->rows);
      }
      free(conn->db);
    }
    free(conn);
  }
}

void Database_write(struct Connection *conn) {
  rewind(conn->file);

  int rc = fwrite(&conn->db->cnt, sizeof(int), 1, conn->file);
  if (rc != 1)
    die(conn, "Failed to write database cnt.");

  for (int i = 0; i < conn->db->cnt; i++) {
    struct Address *addr = &conn->db->rows[i];

    rc = fwrite(&addr->id, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to write id.");
    rc = fwrite(&addr->set, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to write set.");
    rc = fwrite(&addr->data, sizeof(int), 1, conn->file);
    if (rc != 1)
      die(conn, "Failed to write data.");

    rc = fwrite(addr->name, sizeof(char), addr->data, conn->file);
    if (rc != addr->data)
      die(conn, "Failed to write name.");
    rc = fwrite(addr->email, sizeof(char), addr->data, conn->file);
    if (rc != addr->data)
      die(conn, "Failed to write email.");
  }

  rc = fflush(conn->file);
  if (rc == -1)
    die(conn, "Cannot flush database.");
}

void Database_create(int rows, struct Connection *conn) {
  int i = 0;

  for (i = 0; i < rows; i++) {
    conn->db->rows[i].id = i;
    conn->db->rows[i].set = 0;
  }
}

void Database_set(int data, struct Connection *conn, int id, const char *name,
                  const char *email) {
  struct Address *addr = &conn->db->rows[id];
  if (addr->set)
    die(conn, "Already set, delete it first");

  addr->set = 1;
  // WARNING: bug, read the "How To Break It" and fix this
  char *res = strncpy(addr->name, name, data - 1);
  addr->name[data - 1] = '\0';
  // demonstrate the strncpy bug
  if (!res)
    die(conn, "Name copy failed");

  res = strncpy(addr->email, email, data - 1);
  addr->email[data - 1] = '\0';
  if (!res)
    die(conn, "Email copy failed");
}

void Database_get(struct Connection *conn, int id) {
  struct Address *addr = &conn->db->rows[id];

  if (addr->set) {
    Address_print(addr);
  } else {
    die(conn, "ID is not set");
  }
}

void Database_find(struct Connection *conn, const char *name) {
  int is_found = 0;
  for (int i = 0; i < conn->db->cnt; i++) {
    struct Address *addr = &conn->db->rows[i];
    if (addr->set) {
      if (strcmp(name, addr->name) == 0) {
        Address_print(addr);
        is_found = 1;
        break;
      }
    }
  }
  if (!is_found)
    die(conn, "name not found");
}

void Database_delete(struct Connection *conn, int id) {
  struct Address addr = {.id = id, .set = 0};
  conn->db->rows[id].id = id;
  conn->db->rows[id].set = 0;
}

void Database_list(int rows, struct Connection *conn) {
  int i = 0;
  struct Database *db = conn->db;

  for (i = 0; i < rows; i++) {
    struct Address *cur = &db->rows[i];

    if (cur->set) {
      Address_print(cur);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 5)
    die(NULL, "USAGE: ex17 <dbfile> <action> [action params]");

  int data = atoi(argv[1]), rows = atoi(argv[2]);

  char *filename = argv[3];
  char action = argv[4][0];
  conn = Database_open(data, rows, filename, action);
  int id = 0;

  if (argc > 5)
    id = atoi(argv[5]);
  if (id >= rows)
    die(conn, "There's not that many records.");

  switch (action) {
  case 'c':
    Database_create(rows, conn);
    Database_write(conn);
    break;

  case 'g':
    if (argc != 6)
      die(conn, "Need an id to get");

    Database_get(conn, id);
    break;

  case 's':
    if (argc != 8)
      die(conn, "Need id, name, email to set");

    Database_set(data, conn, id, argv[6], argv[7]);
    Database_write(conn);
    break;

  case 'd':
    if (argc != 6)
      die(conn, "Need id to delete");

    Database_delete(conn, id);
    Database_write(conn);
    break;

  case 'l':
    Database_list(rows, conn);
    break;

  case 'f':
    if (argc != 6)
      die(conn, "Need a name to find");
    Database_find(conn, argv[5]);
    break;
  default:
    die(conn, "Invalid action, only: c=create, g=get, s=set, d=del, l=list");
  }

  Database_close(conn);

  return 0;
}

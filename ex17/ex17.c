#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

struct Address {
  int id;
  int set;
  char *name;
  char *email;
};

struct Database {
  struct Address *rows;
  int max_data;
  int max_rows;
};

struct Connection {
  FILE *file;
  struct Database *db;
};

void Database_close(struct Connection *connection);

void die(const char *message, struct Connection *connection)
{
  Database_close(connection);
  if(errno) {
    perror(message);
  } else {
    printf("ERROR: %s\n", message);
  }

  exit(1);
}

void Address_print(struct Address *address)
{
  printf("%d %s %s\n",
      address->id, address->name, address->email);
}

void Database_load(struct Connection *connection)
{
  int read_count = fread(connection->db, sizeof(struct Database), 1, connection->file);
  if(read_count != 1){ die("Failed to load database.", connection); }

  struct Address *rows = malloc(sizeof(struct Address) * connection->db->max_rows);
  if(!rows){ die("Memory Error", connection); }
  connection->db->rows = rows;

  int i;
  struct Address *row;
  for(i=0; i< connection->db->max_rows; i++){
    row = &connection->db->rows[i];
    read_count = fread(row, sizeof(struct Address), 1, connection->file);
    if(read_count != 1){ die("Failed to load database.", connection); }

    row->name = malloc(connection->db->max_data);
    if(!row->name){ die("Memory Error", connection); }

    read_count = fread(row->name, connection->db->max_data, 1, connection->file);
    if(read_count != 1){ die("Failed to load database.", connection); }

    row->email = malloc(connection->db->max_data);
    if(!row->email){ die("Memory Error", connection); }

    read_count = fread(row->email, connection->db->max_data, 1, connection->file);
    if(read_count != 1){ die("Failed to load database.", connection); }
  }
}

struct Connection *Database_open(const char *filename, char mode)
{
  struct Connection *conn = malloc(sizeof(struct Connection));
  if(!conn){ die("Memory Error", conn); }

  conn->db = malloc(sizeof(struct Database));
  if(!conn->db){ die("Memory Error", conn); }

  if(mode == 'c'){
    conn->file = fopen(filename, "w");
    conn->db->max_rows = 0;
    conn->db->max_data = 0;
  } else {
    conn->file = fopen(filename, "r+");

    if(conn->file){ Database_load(conn); }
  }

  if(!conn->file){ die("Failed to open the file", conn); }

  return conn;
}

void Database_close(struct Connection *connection)
{
  if(connection) {
    if(connection->file){ fclose(connection->file); }
    if(connection->db){
      if(connection->db->rows){
        int i;
        for(i = 0; i < connection->db->max_rows; i++){
          struct Address *addr = &connection->db->rows[i];
          if(addr->name){ free(addr->name); }
          if(addr->email){ free(addr->email); }
        }
        free(connection->db->rows);
      }
      free(connection->db);
    }
    free(connection);
  }
}

void Database_write(struct Connection *connection)
{
  rewind(connection->file);
  int write_count = fwrite(connection->db, sizeof(struct Database), 1, connection->file);
  if(write_count != 1){ die("Failed to write database.", connection); }

  int i;
  for(i=0; i < connection->db->max_rows; i++){
    struct Address *row = &connection->db->rows[i];
    write_count = fwrite(row, sizeof(struct Address), 1, connection->file);
    if(write_count != 1){ die("Failed to write database.", connection); }

    if(row->name){
      write_count = fwrite(row->name, connection->db->max_data, 1, connection->file);
      if(write_count != 1){ die("Failed to write database.", connection); }
    }

    if(row->email){
      write_count = fwrite(row->email, connection->db->max_data, 1, connection->file);
      if(write_count != 1){ die("Failed to write database.", connection); }
    }
  }

  int flush_status = fflush(connection->file);
  if(flush_status == -1){ die("Cannot flush database.", connection); }
}

void Database_create(struct Connection *connection, int max_rows, int max_data)
{
  int i;

  connection->db->max_rows = max_rows;
  connection->db->max_data = max_data;

  struct Address *rows = malloc(sizeof(struct Address) * max_rows);
  if(!rows){ die("Memory Error", connection); }
  connection->db->rows = rows;

  for(i=0; i< connection->db->max_rows; i++){
    // make a prototype to initialize it
    struct Address addr = {.id = i, .set = 0};
    // then just assign it
    connection->db->rows[i] = addr;

    struct Address *row = &connection->db->rows[i];

    row->name = malloc(max_data);
    row->email = malloc(max_data);

    if(!row->name || !row->email){ die("Memory Error", connection); }

    memset(row->name, '\0', max_data);
    memset(row->email, '\0', max_data);
  }
}

void Database_set(struct Connection *connection, int id,
                  const char*name, const char *email)
{
  struct Address *addr = &connection->db->rows[id];
  if(addr->set){ die("Already set, delete if first", connection); }

  if(addr->name){ free(addr->name); }
  if(addr->email){ free(addr->email); }

  addr->set = 1;

  addr->name = malloc(connection->db->max_data);
  if(!addr->name){die("Memeory Error", connection);}

  char *res = strncpy(addr->name, name, connection->db->max_data);
  addr->name[connection->db->max_data -1] = '\0';
  if(!res){ die("Name copy failed", connection); }

  addr->email = malloc(connection->db->max_data);
  if(!addr->email){die("Memory Error", connection);}

  res = strncpy(addr->email, email, connection->db->max_data);
  addr->email[connection->db->max_data -1] = '\0';
  if(!res){ die("Email copy failed", connection); }
}

void Database_get(struct Connection *connection, int id)
{
  struct Address *addr = &connection->db->rows[id];
  if(addr->set) {
    Address_print(addr);
  } else {
    die("ID is not set", connection);
  }
}

void Database_delete(struct Connection *connection, int id)
{
  struct Address *old_addr = &connection->db->rows[id];

  if(old_addr->name){ free(old_addr->name); }
  if(old_addr->email){ free(old_addr->email); }

  struct Address addr = {.id = id, .set = 0 };
  connection->db->rows[id] = addr;
}

void Database_list(struct Connection *connection)
{
  int i;
  struct Database *db = connection->db;

  for(i=0; i < db->max_rows; i++) {
    struct Address *current_address = &db->rows[i];

    if(current_address->set) {
      Address_print(current_address);
    }
  }
}

int main(int argc, char *argv[])
{
  if(argc < 3){ die("USAGE: ex17 <dbfile> <action> [action params]", NULL); }

  char *filename = argv[1];
  char action = argv[2][0];
  struct Connection *conn = Database_open(filename, action);
  int id = 0;

  if(action != 'c'){
    if(argc > 3){ id = atoi(argv[3]); }
    if(id >= conn->db->max_rows){ die("There's not that many records.", conn); }
  }

  switch(action) {
    case 'c':
      if(argc != 5){ die("Need max rows and max data", conn); }
      Database_create(conn, atoi(argv[3]), atoi(argv[4]));
      Database_write(conn);
      break;
    case 'g':
      if(argc != 4){ die("Need an id to get", conn); }

      Database_get(conn, id);
      break;
    case 's':
      if(argc != 6){ die("Need an id, name, email to set", conn); }
      Database_set(conn, id, argv[4], argv[5]);
      Database_write(conn);
      break;
    case 'd':
      if(argc != 4){ die("Need id to delete", conn); }
      Database_delete(conn, id);
      Database_write(conn);
      break;
    case 'l':
      Database_list(conn);
      break;
    default:
      die("Invalid action, only: c=create, g=get, s=set, d=del, l=list", conn);
  }

  Database_close(conn);

  return 0;
}

"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import sys
import LoanData
import utilities


class SqliteLCLoanData(LoanData.LCLoanData):
    def __init__(self, conversion_filters, args, worker_idx):
        LoanData.LCLoanData.__init__(self, conversion_filters, args, worker_idx)
        self.con = None
        self.cursor = None

    def load_data(self):
        LoanData.LCLoanData.load_data(self)

        db_name = "loans_serialized_python%d.db" % sys.version_info.major

        reload_db_files = utilities.reload_cached_file(db_name, self.args.stats.split(','))

        if reload_db_files:
            # We have to regenerate the loans using the base class
            LoanData.LCLoanData.load_data(self)

            self.info_msg("Creating memory sqlite3 from loans ...")
            con, cursor = self.create_sqlite_memdb(self.loans)
            self.info_msg("Creating memory sqlite3 from loans done ...")

            self.info_msg("Creating cached sqlite3 %s ..." % db_name)
            self.dump_sqlite_memdb(con, db_name)
            self.info_msg("Creating cached sqlite3 %s ... done" % db_name)
        else:
            self.info_msg("Initializing sqlite3 from cached %s ..." % db_name)
            con, cursor = self.load_sqlite_memdb(db_name)
            self.info_msg("Initializing sqlite3 from cached %s done ..." % db_name)

        self.con = con
        self.cursor = cursor

    def open_sqlite(self, db_name):
        con = utilities.sqlite.connect(db_name)
        cursor = con.cursor()
        return con, cursor

    def load_sqlite_memdb(self, db_name):
        # Read database to tempfile
        con = utilities.sqlite.connect(db_name)
        temp_file = utilities.stringio()
        for line in con.iterdump():
            temp_file.write(line+'\n')
        temp_file.seek(0)
        con.close()

        # Create a database in memory and import from tempfile
        mem_db = utilities.sqlite.connect(":memory:")
        mem_db.cursor().executescript(temp_file.read())
        mem_db.commit()
        mem_db_cursor = mem_db.cursor()
        return mem_db, mem_db_cursor

    def dump_sqlite_memdb(self, con, db_name):
        temp_file = utilities.stringio()
        for line in con.iterdump():
            temp_file.write(line+'\n')
        temp_file.seek(0)

        # Create a database in a file and import from tempfile
        mem_db = utilities.sqlite.connect(db_name)
        mem_db.cursor().executescript(temp_file.read())
        mem_db.commit()
        mem_db.close()
        return con

    def create_sqlite_memdb(self, loans):

        con = utilities.sqlite.connect(":memory:")
        cursor = con.cursor()
        cursor.execute("PRAGMA temp_store = MEMORY")
        cursor.execute("PRAGMA count_changes = OFF")
        cursor.execute("PRAGMA PAGE_SIZE = 4096")
        cursor.execute("DROP TABLE IF EXISTS Loans")

        create_sql = "CREATE TABLE Loans("
        insert_sql = "INSERT INTO  Loans VALUES("

        for lc_filter in self.filters:
            create_sql += lc_filter.name + " " + lc_filter.sqlite_type + ", "
            insert_sql += "?, "
        create_sql = create_sql[:-2] + ")"
        # print(create_sql)
        cursor.execute(create_sql)

        insert_sql = insert_sql[:-2] + ")"
        # print(insert_sql)
        num_filters = len(self.filters)
        cursor.executemany(insert_sql, (loan[:num_filters] for loan in loans))
        con.commit()

        for lc_filter in self.filters:
            index_sql = "CREATE INDEX index_%s ON Loans(%s)" % (lc_filter.name, lc_filter.name)
            cursor.execute(index_sql)
            con.commit()
        return con, cursor

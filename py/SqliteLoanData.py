"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import sys
import sqlite3
import LoanData, utilities


def init_sqlite_db(db_name):
    # Read database to tempfile
    con = sqlite3.connect(db_name)
    temp_file = utilities.stringio()
    for line in con.iterdump():
        temp_file.write('%s\n' % line)
    con.close()
    temp_file.seek(0)

    # Create a database in memory and import from tempfile
    mem_db = sqlite3.connect(":memory:")
    mem_db.cursor().executescript(temp_file.read())
    mem_db.commit()
    return mem_db

class SqliteLCLoanData(LoanData.LCLoanData):
    def __init__(self, conversion_filters, args, worker_idx):
        LoanData.LCLoanData.__init__(self, conversion_filters, args, worker_idx)

    def mid_stage_initialize(self):
        self.info_msg("Initializing sqlite")
        self.create_sqlite_db(self.loans)

    def create_sqlite_db(self, loans):
        #db_name = "loans_serialized_python%d.db" % sys.version_info.major

        self.con = sqlite3.connect(":memory:")
        self.cursor = self.con.cursor()
        self.cursor.execute("PRAGMA temp_store = MEMORY")
        self.cursor.execute("PRAGMA count_changes = OFF")
        self.cursor.execute("PRAGMA PAGE_SIZE = 4096")
        self.cursor.execute("DROP TABLE IF EXISTS Loans")

        create_sql = "CREATE TABLE Loans("
        insert_sql = "INSERT INTO  Loans VALUES("

        for filter in self.filters:
            create_sql += filter.name + " " + filter.sqlite_type + ", "
            insert_sql += "?, "
        create_sql = create_sql[:-2] + ")"
        #print(create_sql)
        self.cursor.execute(create_sql)

        insert_sql = insert_sql[:-2] + ")"
        #print(insert_sql)
        num_filters = len(self.filters)
        self.cursor.executemany(insert_sql, (loan[:num_filters] for loan in loans))
        self.con.commit()

        for filter in self.filters:
            index_sql = "CREATE INDEX index_%s ON Loans(%s)" % (filter.name, filter.name)
            self.cursor.execute(index_sql)
            self.con.commit()

    def open_sqlite_db(self):
        db_name = "loans_serialized_python%d.db" % sys.version_info.major
        self.info_msg("Initializing sqlite3 %s" % db_name)

        if utilities.check_for_sqlite():
            self.con = init_sqlite_db(db_name)
        else:
            self.con = sqlite3.connect(db_name)
        self.cursor = self.con.cursor()

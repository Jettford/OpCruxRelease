#ifndef __GAMECACHE__INTERFACE__FASTDATABASE_HPP__
#define __GAMECACHE__INTERFACE__FASTDATABASE_HPP__

#include <string>
#include <vector>
#include <stdexcept>
#include <memory>
#include <cmath>
#include <list>

namespace GameCache::Interface::FDB {

	class Connection;

	enum class DATA_TYPE : uint32_t {
		NOTHING = 0,
		INTEGER,
		UNKNOWN1,
		FLOAT,
		TEXT,
		BOOLEAN,
		BIGINT,
		UNKNOWN2,
		VARCHAR,
		INVALID_POINTER = 0xffffffff
	};

	class RowData;
	class FieldValue;

	class PointerString {
	private:
		Connection * conn;
		unsigned char * memlocation;
		uint32_t length;
	public:
		PointerString();
		PointerString(Connection * connection, unsigned char * where);
		PointerString(FieldValue * fieldValue);
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		operator std::string() const {
			return std::string(reinterpret_cast<const char*>(memlocation), length);
		}

		operator std::string_view() const {
			return std::string_view(reinterpret_cast<const char*>(memlocation), length);
		}
	};

	class FieldValue {
	private:
		Connection * conn;
		DATA_TYPE dataType = DATA_TYPE::INVALID_POINTER;
		int32_t * data;
		static const std::uint64_t NULL_DATA = 0ULL;


		std::int64_t GetInt64();
	public:
		FieldValue(Connection * connection, DATA_TYPE type, int32_t * where) : conn(connection), dataType(type), data(where) {}
		Connection * getConnection() { return conn; }
		unsigned char* getMemoryLocation();
		std::string ToString();


		inline bool isNull() {
			return dataType == DATA_TYPE::NOTHING;
		}

		operator std::string() {
			return ToString();
		}

		operator PointerString() {
			if (isNull()) {
				return PointerString(conn, nullptr);
			}
			return PointerString(conn, getMemoryLocation());
		}

		operator std::int32_t() {
			if (isNull()) {
				return 0;
			}
			return *reinterpret_cast<std::int32_t*>(getMemoryLocation());
		}

		operator std::float_t() {
			if (isNull()) {
				return 0;
			}
			return *reinterpret_cast<std::float_t*>(getMemoryLocation());
		}

		operator bool() {
			return !isNull() && *reinterpret_cast<std::int32_t*>(getMemoryLocation()) == 1;
		}

		operator std::int64_t() {
			if (isNull()) {
				return 0;
			}
			return GetInt64();
		}
	};

	class ColumnData {
	private:
		Connection * conn;
		unsigned char * memlocation;
	public:
		ColumnData(Connection * connection, unsigned char * where) : conn(connection), memlocation(where) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		DATA_TYPE getColumnDataType(uint32_t indexOfColumn);
		PointerString getColumnName(uint32_t indexOfColumn);
	};

	class ColumnHeader {
	private:
		Connection * conn;
		unsigned char* memlocation;
	public:
		ColumnHeader(Connection * connection, unsigned char * where) : conn(connection), memlocation(where) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		uint32_t getColumnCount();
		PointerString getTableName();
		ColumnData getColumnData();
	};

	class RowData {
	private:
		Connection * conn;
		unsigned char * memlocation;
	public:
		RowData(Connection * connection, unsigned char * where) : conn(connection), memlocation(where) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		DATA_TYPE getColumnDataType(uint32_t indexOfColumn);
		FieldValue getColumnData(uint32_t indexOfColumn);
	};

	class RowDataHeader {
	private:
		Connection * conn;
		unsigned char * memlocation;
	public:
		RowDataHeader(Connection * connection, unsigned char * where): conn(connection), memlocation(where) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		bool isRowDataValid();
		int32_t getColumnCount();
		RowData getRowData();

		DATA_TYPE getColumnDataType(uint32_t indexOfColumn) { return getRowData().getColumnDataType(indexOfColumn); }
		FieldValue getColumnData(uint32_t indexOfColumn) { return getRowData().getColumnData(indexOfColumn); }
	};

	class RowInfo {
	private:
		Connection * conn;
		unsigned char * memlocation;
		unsigned char * columnHeaderAddr;
	public:
		RowInfo() : conn(nullptr), memlocation(nullptr), columnHeaderAddr(nullptr) {}
		RowInfo(Connection * connection, unsigned char * where, unsigned char * columnHeader): conn(connection), memlocation(where), columnHeaderAddr(columnHeader) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		bool isValid();
		bool isRowDataHeaderValid();
		bool isLinkedRowInfoValid();

		RowDataHeader getRowDataHeader();
		RowInfo	getLinkedRowInfo();
		RowInfo getLinkedRowInfoUnsafe();

		RowData getRowData() { return getRowDataHeader().getRowData(); }
			DATA_TYPE getColumnDataType(uint32_t indexOfColumn) { return getRowData().getColumnDataType(indexOfColumn); }
			FieldValue getRowData(uint32_t indexOfColumn) { return getRowData().getColumnData(indexOfColumn); }
			int32_t getColumnCount() { return getRowDataHeader().getColumnCount(); }
			PointerString getColumnName(uint32_t indexOfColumn) { return ColumnHeader(conn, columnHeaderAddr).getColumnData().getColumnName(indexOfColumn); }

		FieldValue operator [] (uint32_t indexOfColumn) {
			return getRowData(indexOfColumn);
		}

		std::list<RowInfo> flatIt() {
			if (this->memlocation == nullptr) return {};
			std::list<RowInfo> rList = { *this };
			RowInfo tmpRI = *this;
			while (tmpRI.isLinkedRowInfoValid()) {
				tmpRI = tmpRI.getLinkedRowInfo();
				rList.push_back(tmpRI);
			}
			return rList;
		}

		FieldValue operator [] (std::string columnName) {
			ColumnHeader columnHeader = ColumnHeader(conn, columnHeaderAddr);
			int32_t column_count = getRowDataHeader().getColumnCount();
			for (std::ptrdiff_t i = 0; i < column_count; ++i) {
				if (static_cast<std::string>(columnHeader.getColumnData().getColumnName(i)) == columnName) {
					return getRowData(i);
				}
			}
			throw std::runtime_error("Field \"" + columnName + "\" can not be found.");
		}
	};

	class RowHeader {
	private:
		Connection * conn;
		unsigned char * memlocation;
		unsigned char * columnHeaderAddr;
	public:
		RowHeader(Connection * connection, unsigned char * where, unsigned char * columnHeader) : conn(connection), memlocation(where), columnHeaderAddr(columnHeader) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		RowInfo getRowInfo(uint32_t indexOfRow);
		bool isRowInfoValid(uint32_t indexOfRow);
	};

	class RowTopHeader {
	private:
		Connection * conn;
		unsigned char * memlocation;
		unsigned char * columnHeaderAddr;
	public:
		RowTopHeader(Connection * connection, unsigned char * where, unsigned char * columnHeader) : conn(connection), memlocation(where), columnHeaderAddr(columnHeader) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		uint32_t getRowCount();
		RowHeader getRowHeader();
		bool isRowHeaderValid();

		RowInfo operator [] (uint32_t indexOfRow) {
			return getRowHeader().getRowInfo(indexOfRow);
		}

		bool isValid(uint32_t indexOfRow) {
			return getRowHeader().isRowInfoValid(indexOfRow);
		}
	};

	class TableHeader {
	private:
		Connection * conn;
		unsigned char* memlocation;
	public:
		TableHeader(Connection * connection, unsigned char * where) : conn(connection), memlocation(where) {}
		Connection * getConnection() { return conn; }
		unsigned char * getMemoryLocation() { return memlocation; }

		ColumnHeader getColumnHeader(uint32_t indexOfTable);
		RowTopHeader getRowTopHeader(uint32_t indexOfTable);

		PointerString getTableName(uint32_t indexOfTable) { return getColumnHeader(indexOfTable).getTableName(); }
	};

	typedef std::vector<RowInfo> QueryResult;

	class Connection {
	private:
		std::shared_ptr<unsigned char[]> filePtr;
		unsigned char * fileData = nullptr;
	public:
		Connection(std::string database);
		Connection() {}
		void Connect(std::string database);
		bool isConnected() { return fileData != nullptr; }
		uint32_t getTableCount();

		unsigned char * getFileData();

		TableHeader getTableHeader();

		uint32_t getTableIndex(std::string tableName);
		ColumnHeader getColumns(uint32_t indexOfTable) { return getTableHeader().getColumnHeader(indexOfTable); }
		RowTopHeader getRows(uint32_t indexOfTable);
		RowTopHeader getRows(std::string tableName) { return getRows(getTableIndex(tableName)); }

		QueryResult Query(uint32_t indexOfTable, bool compare(std::string columnName, std::string columnVal));
		QueryResult Query(std::string tablename, bool compare(std::string columnName, std::string columnVal));

		~Connection() {}
	};
}


#define CRUX_CACHE_ADD_COLUMN_GETTER(index, type, name) inline type Get##name(FDB::RowInfo row) {return row[index];}inline type Get##name(std::int32_t id) {return Get##name(getRow(id));}\

#define CRUX_CACHE_ADD_COLUMN_GETTER_MR(index, type, name) inline type Get##name(FDB::RowInfo row) {return row[index];}\


#endif // !__GAMECACHE__INTERFACE__FASTDATABASE_HPP__

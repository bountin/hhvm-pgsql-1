#ifndef incl_HPHP_PDO_PGSQL_STATEMENT_H_
#define incl_HPHP_PDO_PGSQL_STATEMENT_H_

#include "hphp/runtime/ext/pdo_driver.h"
#include "pq.h"
#include "stdarg.h"

#define BOOLOID     16
#define BYTEAOID    17
#define INT8OID     20
#define INT2OID     21
#define INT4OID     23
#define TEXTOID     25
#define OIDOID      26

namespace HPHP {
    class PDOPgSqlConnection;
    class PDOPgSqlStatement : public PDOStatement {
        friend class PDOPgSqlConnection;
    public:
        DECLARE_RESOURCE_ALLOCATION(PDOPgSqlStatement);
        PDOPgSqlStatement(PDOPgSqlConnection* m_pdoconn, PQ::Connection* pq);
        virtual ~PDOPgSqlStatement();

        bool create(const String& sql, const Array &options);

        virtual bool executer();
        virtual bool fetcher(PDOFetchOrientation ori, long offset);

        virtual bool describer(int colno);

        virtual bool getColumnMeta(int64_t colno, Array &return_value);
        virtual bool getColumn(int colno, Variant &value);

        virtual bool paramHook(PDOBoundParam* param, PDOParamEvent event_type);

        virtual bool cursorCloser();

        virtual bool support(SupportedMethod method);
    private:
        PDOPgSqlConnection* m_pdoconn;
        PQ::Connection* m_conn;
        static unsigned long m_stmtNameCounter;
        static unsigned long m_cursorNameCounter;
        std::string m_stmtName;
        std::string m_resolvedQuery;
        std::string m_cursorName;
        std::string err_msg;
        PQ::Result m_result;
        bool m_isPrepared;
        bool m_hasParams;

        std::vector<Oid> param_types;
        std::vector<Variant> param_values;
        std::vector<int> param_lengths;
        std::vector<int> param_formats;

        std::vector<Oid> m_pgsql_column_types;

        long m_current_row;

        std::string strprintf(const char* format, ...){
            va_list args, args_copy;
            va_start (args, format);
            va_copy (args_copy, args);
            int size = vsnprintf(NULL, 0, format, args_copy);
            auto buffer = std::unique_ptr<char[]>(new char[size+1]);
            if(vsnprintf((char*)buffer.get(), size+1, format, args) != size){
                throw std::exception();
            }
            va_end (args);
            return std::string((char*)buffer.get());
        }

        std::string oriToStr(PDOFetchOrientation ori, long offset, bool& success){
            success = true;
            switch(ori){
                case PDO_FETCH_ORI_NEXT:
                    return std::string("NEXT");
                case PDO_FETCH_ORI_PRIOR:
                    return std::string("BACKWARD");
                case PDO_FETCH_ORI_FIRST:
                    return std::string("FIRST");
                case PDO_FETCH_ORI_LAST:
                    return std::string("LAST");
                case PDO_FETCH_ORI_ABS:
                    return strprintf("ABSOLUTE %ld", offset);
                case PDO_FETCH_ORI_REL:
                    return strprintf("RELATIVE %ld", offset);
                default:
                    success = false;
                    return std::string();
            }
        }
    };

}

#endif

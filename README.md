# mysql_dbcp
database connection pool for mysql

## 初衷

>之所以基于mysql-connector-c再重新封装一个mysql数据库连接池而不使用现成的mysql-connector-c++，有两个原因，一个是
mysql-connector-c++要依赖于boost库，而我们的开发库不想引入boost这么庞大的库，很多库在c++11里面已经能够找到替代；
然后就是mysql-connector-c++的错误处理是以异常的方式来体现，我觉得这种设计不是很友好，在c++里面的异常处理就是耍流氓；
这里面改用错误码的方式给出错误提示

## examples

```
#include <iostream>
#include <dbcp.hpp>
#include <time.h>
#include <random>

namespace dbcp{

    static std::string remote_host = "127.0.0.1";

    void test_query(){
        printf("test_query.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        detail::datasource_factory* ds_factory = detail::datasource_factory::create_instance();
        datasource* ds = ds_factory->create("mysql", config, 1, 2);
        if (!ds){
            printf("create datasource failed\n");
            return;
        }

        connection_wrapper con(ds->get_connection());
        if (!con){
            printf("get connection from datasource failed\n");
            return;
        }

        resultset_ptr result = con->query("select * from test_userinfo");
        if (!result){
            printf("result is null\n");
            return;
        }

        resultset_metadata* meta_data = result->meta_data();
        if (!meta_data){
            printf("meta is null\n");
            return;
        }

        int32_t num_field = meta_data->get_column_count();
        printf("num of field is %d\n", num_field);
        for (int32_t i = 0; i < num_field; ++i){
            printf("filed %d is %-20s, type: %d\n",
                i, meta_data->get_column_name(i).c_str(), meta_data->get_column_type(i));
        }

        printf("rows: %lld\n", result->row_count());

        while (result->move_next()){
            for (int32_t i = 0; i < num_field; ++i){
                printf("col %d, name %-20s, value %s\n",
                    i, meta_data->get_column_name(i).c_str(), result->get_string(i).c_str());
            }
            printf("-----------------------------------\n");
        }

        printf("get by name test\n");
        result->before_first();
        while (result->move_next()){
            printf("user_id             , value %lld\n", result->get_int64("user_id"));
            printf("user_name           , value %s\n", result->get_string("user_name").c_str());
            printf("user_image          , value %d\n", result->get_int32("user_image"));
            printf("user_score          , value %.2f\n", result->get_double("user_score"));
            printf("last_modify_time    , value %s\n", result->get_string("last_modify_time").c_str());
            printf("------------------------------------\n");
        }
    }

    void test_insert(){
        printf("test_insert.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        char sql[1204] = { 0 };

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;
            char user_name[256] = { 0 };
            sprintf(user_name, "user_%lld", user_id);

            sprintf(sql,
                "insert into test_userinfo(user_id, user_name, user_image, user_score, last_modify_time) "
                "values(%lld, '%s', %d, %f, now())",
                user_id, user_name, 0, (double)(rand() % 3000));

            printf("sql : {%s}\n", sql);
            
            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(sql, err);
            printf("affect rows :%d, error{%d, %s}\n", 
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_insert_on_duplicate(){
        printf("test_insert_on_duplicate.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        char sql[1204] = { 0 };

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;
            char user_name[256] = { 0 };
            sprintf(user_name, "user_%lld", user_id);

            sprintf(sql,
                "insert into test_userinfo(user_id, user_name, user_image, user_score, last_modify_time) "
                "values(%lld, '%s', %d, %f, now()) on duplicate key update last_modify_time = now()",
                user_id, user_name, 0, (double)(rand() % 3000));

            printf("sql : {%s}\n", sql);

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(sql, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_update(){
        printf("test_update.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        char sql[1204] = { 0 };

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;

            sprintf(sql,
                "update test_userinfo set user_image = %d, last_modify_time = now() where user_id = %lld",
                rand() % 100, user_id);

            printf("sql : {%s}\n", sql);

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(sql, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_delete(){
        printf("test_delete.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        char sql[1204] = { 0 };

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;

            sprintf(sql,
                "delete from test_userinfo where user_id = %lld", user_id);

            printf("sql : {%s}\n", sql);

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(sql, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }
    

    /** prepare interface */

    void test_prepared_query(){
        printf("test_prepared_query.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        prepared_statement_ptr stmt = con.prepared_statement("select * from test_userinfo");
        error_code err;
        resultset_ptr result = con.query(stmt, err);
        if (!result){
            printf("result is null\n");
            return;
        }

        resultset_metadata* meta_data = result->meta_data();
        if (!meta_data){
            printf("meta is null\n");
            return;
        }

        int32_t num_field = meta_data->get_column_count();
        printf("num of field is %d\n", num_field);
        for (int32_t i = 0; i < num_field; ++i){
            printf("filed %d is %-20s, type: %d\n",
                i, meta_data->get_column_name(i).c_str(), meta_data->get_column_type(i));
        }

        printf("rows: %lld\n", result->row_count());

        while (result->move_next()){
            for (int32_t i = 0; i < num_field; ++i){
                printf("col %d, name %-20s, value %s\n",
                    i, meta_data->get_column_name(i).c_str(), result->get_string(i).c_str());
            }
            printf("-----------------------------------\n");
        }

        //printf("get by name test\n");
        //result->before_first();
        //while (result->move_next()){
        //    printf("user_id             , value %lld\n", result->get_int64("user_id"));
        //    printf("user_name           , value %s\n", result->get_string("user_name").c_str());
        //    printf("user_image          , value %d\n", result->get_int32("user_image"));
        //    printf("user_score          , value %.2f\n", result->get_double("user_score"));
        //    printf("last_modify_time    , value %s\n", result->get_string("last_modify_time").c_str());
        //    printf("------------------------------------\n");
        //}
    }

    void test_prepared_query_with_param(){
        printf("test_prepared_query_with_param.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        prepared_statement_ptr stmt = con.prepared_statement("select * from test_userinfo where user_id = ? or user_id = ? or user_id = ?");
        int32_t index = 0;
        stmt->set_int64(index++, 1);
        stmt->set_int64(index++, 2);
        stmt->set_int64(index++, 11);

        error_code err;
        resultset_ptr result = con.query(stmt, err);
        if (!result){
            printf("result is null\n");
            return;
        }

        resultset_metadata* meta_data = result->meta_data();
        if (!meta_data){
            printf("meta is null\n");
            return;
        }

        int32_t num_field = meta_data->get_column_count();
        printf("num of field is %d\n", num_field);
        for (int32_t i = 0; i < num_field; ++i){
            printf("filed %d is %-20s, type: %d\n",
                i, meta_data->get_column_name(i).c_str(), meta_data->get_column_type(i));
        }

        printf("rows: %lld\n", result->row_count());

        while (result->move_next()){
            for (int32_t i = 0; i < num_field; ++i){
                printf("col %d, name %-20s, value %s\n",
                    i, meta_data->get_column_name(i).c_str(), result->get_string(i).c_str());
            }
            printf("-----------------------------------\n");
        }

        //printf("get by name test\n");
        //result->before_first();
        //while (result->move_next()){
        //    printf("user_id             , value %lld\n", result->get_int64("user_id"));
        //    printf("user_name           , value %s\n", result->get_string("user_name").c_str());
        //    printf("user_image          , value %d\n", result->get_int32("user_image"));
        //    printf("user_score          , value %.2f\n", result->get_double("user_score"));
        //    printf("last_modify_time    , value %s\n", result->get_string("last_modify_time").c_str());
        //    printf("------------------------------------\n");
        //}
    }

    void test_prepared_insert(){
        printf("test_prepared_insert.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;
            char user_name[256] = { 0 };
            sprintf(user_name, "user_%lld", user_id);

            prepared_statement_ptr stmt = 
                con.prepared_statement("insert into test_userinfo(user_id, user_name, user_image, user_score, last_modify_time) values(?, ?, ?, ?, now())");
            int32_t index = 0;
            stmt->set_int64(index++, user_id);
            stmt->set_string(index++, user_name);
            stmt->set_int32(index++, rand() % 100);
            stmt->set_double(index++, (double)(rand() % 3000));

            error_code err;
            int32_t affect_rows = con.execute(stmt, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_prepared_insert_on_duplicate(){
        printf("test_prepared_insert_on_duplicate.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;
            char user_name[256] = { 0 };
            sprintf(user_name, "user_%lld", user_id);

            prepared_statement_ptr stmt = con.prepared_statement("insert into test_userinfo(user_id, user_name, user_image, user_score, last_modify_time) values(?, ?, ?, ?, now()) on duplicate key update last_modify_time = now()");
            int32_t index = 0;
            stmt->set_int64(index++, user_id);
            stmt->set_string(index++, user_name);
            stmt->set_int32(index++, rand() % 100);
            stmt->set_double(index++, (double)(rand() % 3000));

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(stmt, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_prepared_update(){
        printf("test_prepared_update.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;

            prepared_statement_ptr stmt = con.prepared_statement("update test_userinfo set user_image = ?, last_modify_time = now() where user_id = ?");
            int32_t index = 0;
            stmt->set_int32(index++, rand() % 100);
            stmt->set_int64(index++, user_id);

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(stmt, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void test_prepared_delete(){
        printf("test_prepared_delete.\n");

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        mysql::mysql_connection con;
        if (!con.initialize(config)){
            printf("con initialize failed\n");
            return;
        }

        int64_t start_base_id = 0;
        printf("input base id: ");
        std::cin >> start_base_id;

        srand((uint32_t)time(nullptr));
        for (int32_t i = 0; i < 2; ++i){
            int64_t user_id = start_base_id + i;

            prepared_statement_ptr stmt = con.prepared_statement("delete from test_userinfo where user_id = ?");
            int32_t index = 0;
            stmt->set_int64(index++, user_id);

            //int32_t affect_rows = con.execute(sql);
            error_code err;
            int32_t affect_rows = con.execute(stmt, err);
            printf("affect rows :%d, error{%d, %s}\n",
                affect_rows, err.value(), err.message().c_str());
        }
    }

    void other_test(){
        driver* dr = mysql::create_driver_instance();

        connection_config config;
        config.host = remote_host;
        config.host_port = 3306;
        config.user_name = "root";
        config.user_passwd = "123456";
        config.db_name = "test_db";

        connection* con = dr->connect(config);
        if (!con){
            printf("try connection mysql server[%s] failed\n", config.to_string().c_str());
        }
        else{
            printf("try connection mysql server[%s] suscce\n", config.to_string().c_str());
        }

        delete dr;
    }
}

int main()
{
    printf("dbcp_test\n");

    printf("Set test db host.\n");
    printf("0 - Use default server address[127.0.0.1]\n");
    printf("1 - Manual input server address.\n");

    int32_t i = 0;
    std::cin >> i;

    if (i) {
        std::cin >> dbcp::remote_host;
    }
    else {
        dbcp::remote_host = "127.0.0.1";
    }

    printf("test db remote_host is %s\n\n", dbcp::remote_host.c_str());

    //dbcp::test_insert();

    //dbcp::test_insert_on_duplicate();

    //dbcp::test_update();

    //dbcp::test_delete();

    //dbcp::test_query();

    //dbcp::test_prepared_query();

    //dbcp::test_prepared_query_with_param();

    //dbcp::test_prepared_insert();

    //dbcp::test_prepared_insert_on_duplicate();

    //dbcp::test_prepared_update();

    //dbcp::test_prepared_delete();

    //dbcp::other_test();

    dbcp::test_query();

    system("pause");
    return 0;
}
```

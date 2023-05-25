#include <iostream>
#include <vector>
#include <limits> // cin.ignore
#include <string.h> // strcpy()
#include <iomanip> // setw()
#include <algorithm>
#include <cstring>
#include <hiredis/hiredis.h>

using namespace std;

bool rscon(redisContext*& context, const char* ip = "127.0.0.1", int port = 6379){
    context = redisConnect(ip, port);
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;
        } else {
            cout << "Can't allocate redis context" << endl;
        }
        return false;
    }

    return true;
}

vector<const char*> rsKEYS(redisContext*& context){
    vector<const char*> tables;
    redisReply* reply = (redisReply*)redisCommand(context, "KEYS *:0");
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
    }      

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        for (int i = 0; i < reply->elements; i++) {
            string tmpstr = reply->element[i]->str;
            tmpstr.erase(tmpstr.size()-2, 2);
            char* res = new char[tmpstr.length() + 1];
            strcpy(res, tmpstr.c_str());
            tables.push_back(res);
        }

        freeReplyObject(reply);       
    } else {
        // cout << "No tables" << endl;
        freeReplyObject(reply);
    }
    return tables;
}

bool rsHGETALL(const char* nameTable, int id, redisContext*& context, int setwint = 10){
    redisReply* reply = (redisReply*)redisCommand(context, "HGETALL %s:%d", nameTable, id);
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
    }      

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        cout << "|" << setw(3) << id;
        for (size_t i = 0; i < reply->elements; i += 2) {
            redisReply *field = reply->element[i];
            redisReply *value = reply->element[i + 1];
            cout << "|" << setw(setwint) << value->str;
        }
        cout << "|" << endl;

        freeReplyObject(reply);
        return true;
    } else {

        freeReplyObject(reply);
        return false;
    }
}

char* rsHGET(const char* nameTable, int id, const char* field, redisContext*& context, int setwint = 10){
    redisReply* reply = (redisReply*)redisCommand(context, "HGET %s:%d %s", nameTable, id, field);
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
    }      

    string tmpstr = reply->str;
    char* value = new char[tmpstr.length() + 1];
    strcpy(value, tmpstr.c_str());

    freeReplyObject(reply);
    return value;
}

bool rsHMSET(const char* nameTable, int id, string field, string value, redisContext*& context){
    redisReply* reply = (redisReply*)redisCommand(context, "HMSET %s:%d %s %s", nameTable, id, field.c_str(), value.c_str());
    if (reply == NULL) {
        std::cout << "Error executing HMSET command: " << context->errstr << std::endl;
        redisFree(context);
        return false;
    }

    if (reply->type != REDIS_REPLY_STATUS || std::string(reply->str) != "OK") {
        std::cout << "HMSET command failed: " << reply->str << std::endl;
        return false;
    }

    freeReplyObject(reply);
    return true;
}

vector<char*> rsGetFields(const char* nameTable, int id, redisContext*& context){
    vector<char*> fields;

    redisReply* reply = (redisReply*)redisCommand(context, "HGETALL %s:%d", nameTable, id);
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
    }     

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        for (size_t i = 0; i < reply->elements; i += 2) {
            string tmpstr = reply->element[i]->str;
            char* res = new char[tmpstr.length() + 1];
            strcpy(res, tmpstr.c_str());
            fields.push_back(res);
        }
    } 

    freeReplyObject(reply);  
    return fields;
}

bool rsSave(redisContext*& context){
    redisReply* reply = (redisReply*)redisCommand(context, "SAVE");
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
        return false;
    } 
    return true;
}

bool isEndTable(const char* nameTable, int id, redisContext*& context){
    redisReply* reply = (redisReply*)redisCommand(context, "HGETALL %s:%d", nameTable, id);
    if (context == NULL || context->err) {
        if (context) {
            cout << "Error: " << context->errstr << endl;      
        } else {
            cout << "Can't allocate redis context" << endl;
        }
    }      

    if (reply->type == REDIS_REPLY_ARRAY && reply->elements > 0) {
        freeReplyObject(reply);
        return false;
    } else {

        freeReplyObject(reply);
        return true;
    }
}

bool compareStrings(const char* a, const char* b) {
    return std::strcmp(a, b) < 0;
}

void sortVector(std::vector<const char*>& vec) {
    std::sort(vec.begin(), vec.end(), compareStrings);
}

int getSizeSetw(vector<char*> fields, int width = 130){
    return width / fields.size();
}

int getSizeTable(redisContext*& context, vector<const char*> tables, int choice){
    int id = 1;
    while(!isEndTable(tables[choice], id, context)){
        id++;    
    }
    return id - 1;
}

int getSizeTable(redisContext*& context, const char* nameTable){
    int id = 1;
    while(!isEndTable(nameTable, id, context)){
        id++;    
    }
    return id - 1;
}

int getInt(int limit = 3, int dlimit = 0)
{
    while (true)
    {
        cout << "Enter: ";
        double entered_number;
        cin >> entered_number;

        if (cin.fail() || entered_number < dlimit || entered_number > limit)
        {
            cout << "Invalid input" << endl;
            cin.clear();
            cin.ignore(32767, '\n');
        }
        else
            return entered_number;
    }
}

bool makeDEL(const char* nameTable, int id, char* field, redisContext*& context){
    return rsHMSET(nameTable, id, field, "DELETED", context);
}

bool makeNULL(const char* nameTable, int id, char* field, redisContext*& context){
    return rsHMSET(nameTable, id, field, "NULL", context);
}

void preTable(string nameTable, vector<char*> fields){
    int size = getSizeSetw(fields);
    cout << "|Table: " << nameTable << endl;
    cout << "|" << setw(3) << " # " << "|";
    int k;
    for(k = 0; k < fields.size(); k++){
        cout << setw(size) << fields[k] << "|";
    }
    cout << endl << "=====";
    for(int i = 0; i < k; i++){
        for(int y = 0; y <= size; y++){
            cout << "=";
        }
    }
    cout << endl;
}

void afTable(vector<char*> fields){
    int size = getSizeSetw(fields);
    cout << "=====";
    for(int i = 0; i < fields.size(); i++){
        for(int y = 0; y <= size; y++){
            cout << "=";
        }
    }
    cout << endl;
}

int showTable(const char* nameTable, redisContext*& context){
    vector<char*> fields = rsGetFields(nameTable, 0, context);

    preTable(nameTable, fields);
    int id = 1;
    while(rsHGETALL(nameTable, id, context, getSizeSetw(fields))){
        id++;
    }
    afTable(fields);

    return id - 1; // sizeTable     
}

void editRecordInTable(redisContext*& context, vector<const char*> tables, int choice){
    int sizeTable = getSizeTable(context,tables,choice);

    if(sizeTable > 0){
        cout << "Choose an entry number:" << endl;
        int choice2 = getInt(sizeTable);
        vector<char*> fields;
        fields = rsGetFields(tables[choice], 0, context);

        preTable(tables[choice], fields);
        rsHGETALL(tables[choice], choice2, context, getSizeSetw(fields));
        afTable(fields);

        cout << "Choose an field: " << endl; 
        for(int k = 0; k < fields.size(); k++){
            cout << k << ") " << fields[k] << endl;
        }
        int choice3 = getInt(fields.size() - 1);

        string strfield = fields[choice3];
        string inputValue;
        int intValue;
        if(strfield.find("fk_") != string::npos){
            strfield.erase(0,3);
            strfield.erase(strfield.size()-3, 3);
            strfield += "s";
            const char* charfield = strfield.c_str();

            int sizeTable = getSizeTable(context, charfield);
            if(!sizeTable){
                cout << "First fill in the tables whose keys this table contains" << endl;
                return;
            }
            cout << sizeTable << " records in table " << charfield << endl;
            intValue = getInt(sizeTable,1);
            inputValue = to_string(intValue);
            rsHMSET(tables[choice], choice2, fields[choice3], inputValue, context); 
        } else {
            if(cin.peek() == '\n')
                cin.get();  
            cout << "Enter new value: " << endl;
            getline(cin,inputValue);

            rsHMSET(tables[choice], choice2, fields[choice3], inputValue, context);

            preTable(tables[choice], fields);
            rsHGETALL(tables[choice], choice2, context, getSizeSetw(fields)); 
            afTable(fields);
        }

    } else {
        cout << "Table is empty" << endl;
    }
}

void addRecordInTable(redisContext*& context, vector<const char*> tables, int choice){
    int newId = getSizeTable(context, tables, choice) + 1;
    vector<char*> fields = rsGetFields(tables[choice], 0, context);
    vector<string> strfields;
    int intValue;

    for(int i = 0; i < fields.size(); i++){
        strfields.push_back(fields[i]);
    }
    
    for(int k = 0; k < fields.size(); k++){
        if(strfields[k].find("fk_") != string::npos){
            string inputValue;
            strfields[k].erase(0,3);
            strfields[k].erase(strfields[k].size()-3, 3);
            strfields[k] += "s";
            const char* charfield = strfields[k].c_str();

            int sizeTable = getSizeTable(context, charfield);
            if(!sizeTable){
                cout << "First fill in the tables whose keys this table contains" << endl;
                for(k; k < fields.size(); k++){
                    makeNULL(tables[choice], newId, fields[k], context);
                }
                return;
            }
            cout << "Enter " << fields[k] << ": ";
            cout << sizeTable << " records in table " << charfield << endl;
            intValue = getInt(sizeTable,1);
            inputValue = to_string(intValue);
            rsHMSET(tables[choice], newId, fields[k], inputValue, context);
        } else {
            string inputValue;
            cout << "Enter " << fields[k] << ": ";
            if(cin.peek() == '\n')
                cin.get();    
            getline(cin,inputValue); 
            rsHMSET(tables[choice], newId, fields[k], inputValue, context);
        }

    }   

    preTable(tables[choice], fields);
    rsHGETALL(tables[choice], newId, context, getSizeSetw(fields)); 
    afTable(fields);
}

void searchRecordInTable(redisContext*& context, vector<const char*> tables, int choice){
    int sizeTable = getSizeTable(context,tables,choice);
    if(sizeTable > 0){
        vector<char*> fields = rsGetFields(tables[choice], 0, context);

        cout << "Choose an field: " << endl; 
        for(int k = 0; k < fields.size(); k++){
            cout << k << ") " << fields[k] << endl;
        }
        int choice2 = getInt(fields.size() - 1);

        if(cin.peek() == '\n')
                cin.get();  
        cout << "Enter value: " << endl;
        string inputValue;
        getline(cin,inputValue); 
        vector<int> ids;

        sizeTable = getSizeTable(context, tables, choice);
        for(int id = 1; id <= sizeTable; id++){
            string value = rsHGET(tables[choice], id, fields[choice2], context);
            if(value.find(inputValue) != string::npos){
                ids.push_back(id);
            }      
        }

        cout << "FOUND: " << ids.size() << endl;
        if(ids.size() != 0) {
            preTable(tables[choice], fields);
            for(int i = 0; i < ids.size(); i++){
                rsHGETALL(tables[choice], ids[i], context, getSizeSetw(fields)); 
            }
            afTable(fields);
        }
    } else {
        cout << "Table is empty" << endl;
    }
}

void deletefk(redisContext*& context, vector<const char*> tables, int choice, int choice2){
    for(int i = 0; i < tables.size(); i++){
        string tmp1 = tables[choice];
        tmp1.pop_back();

        vector<char*> fields = rsGetFields(tables[i], 0, context);
        for(int k = 0; k < fields.size(); k++){
            string tmp2 = fields[k];
            if(tmp2.find(tmp1) != string::npos){
                for(int j = 1; j <= getSizeTable(context,tables[i]); j++){
                    string value = rsHGET(tables[i],j,fields[k], context);
                    if(value != "NULL"){
                        int valueint = stoi(value);
                        if(valueint == choice2){
                            rsHMSET(tables[i], j, fields[k], "NULL", context);
                            cout << tables[i] << ":" << j << "." << fields[k] << " = NULL" << endl;
                        }
                    }
                }
            }
        }
    }
}

void deleteRecordInTable(redisContext*& context, vector<const char*> tables, int choice){
    int sizeTable = getSizeTable(context,tables,choice);
    if(sizeTable > 0){
        cout << "Choose an entry number:" << endl;
        int choice2 = getInt(sizeTable);
        vector<char*> fields = rsGetFields(tables[choice], 0, context);

        for(int i = 0; i < fields.size();i++){
            makeDEL(tables[choice], choice2, fields[i], context);
        }                       

        preTable(tables[choice], fields);
        rsHGETALL(tables[choice], choice2, context, getSizeSetw(fields));
        afTable(fields);

        deletefk(context, tables, choice, choice2);
    } else {
        cout << "Table is empty" << endl;
    }
}

void viewTable(redisContext*& context, vector<const char*> tables){
    cout << "Choose an option:" << endl;
    for(int i = 0; i < tables.size(); i++){
        cout << i << ") " << tables[i] << endl;
    }
    int choice = getInt(tables.size() - 1);
    showTable(tables[choice], context);

    cout << "Choose an option: (1)Menu, (2)Edit, (3)Add, (4)Search or (5)Delete" << endl;
    switch (getInt(5, 1)){
        case 1:
            break;
        case 2:
            editRecordInTable(context, tables, choice);                     
            break;
        case 3:
            addRecordInTable(context, tables, choice); 
            break;
        case 4:
            searchRecordInTable(context, tables, choice);
            break;
        case 5:
            deleteRecordInTable(context, tables, choice);           
            break;
        default:
            cout << "Error getInt()";                    
    }

}

void showAllTables(redisContext*& context, vector<const char*> tables){
    for(int i = 0; i < tables.size(); i++){
        showTable(tables[i], context);
    }
}

void viewMergedTable(redisContext*& context, vector<const char*> tables){
    cout << "Choose 2 tables:" << endl;
    for(int i = 0; i < tables.size(); i++){
        cout << i << ") " << tables[i] << endl;
    }

    int choice = getInt(tables.size() - 1);
    int choice2 = -1;
    while((choice == choice2) || choice2 < 0){
        choice2 = getInt(tables.size() - 1);
    }

    string nameTable = tables[choice];
    nameTable.erase(nameTable.size()-1, 1);
    vector<char*> fields1 = rsGetFields(tables[choice], 0, context);
    vector<char*> fields2 = rsGetFields(tables[choice2], 0, context);
    bool isFound = false;
    char* fk_field;
    for(int i = 0; i < fields2.size(); i++){
        string strfields2 = fields2[i];
        if(strfields2.find(nameTable) != string::npos){
            fk_field = fields2[i];
            isFound = true;
        }  
    }

    if(isFound){
        for(int i = 0; i < getSizeTable(context,tables,choice2);i++){
            char* value = rsHGET(tables[choice2], i+1, fk_field, context);
            int valueint;
            try {
            valueint = stoi(value);
            preTable(tables[choice], fields1);
            rsHGETALL(tables[choice], valueint, context, getSizeSetw(fields1));
            afTable(fields1);

            preTable(tables[choice2], fields2);
            rsHGETALL(tables[choice2], i+1, context, getSizeSetw(fields2));
            afTable(fields2);
            cout << endl;
            } catch(exception const& e){
                
            }
        }  
    } else {
        cout << tables[choice2] << " doesn`t have FK of the table " << tables[choice] << endl;
    }
}

bool generationTables(redisContext*& context){
    vector<const char*> tables;

    redisReply* reply = (redisReply*)redisCommand(context, "flushdb");
    reply = (redisReply*)redisCommand(context, "hmset patients:0 fullName 1 dateBirth 1");
    reply = (redisReply*)redisCommand(context, "hmset diagnoses:0 diadnosis 1");
    reply = (redisReply*)redisCommand(context, "hmset positions:0 position 1");
    reply = (redisReply*)redisCommand(context, "hmset hospitals:0 name 1");
    reply = (redisReply*)redisCommand(context, "hmset departments:0 name 1 fk_hospital_id 1");
    reply = (redisReply*)redisCommand(context, "hmset rooms:0 number 1 fk_department_id 1");
    reply = (redisReply*)redisCommand(context, "hmset doctors:0 fullName 1 dateBirth 1 fk_position_id 1 fk_department_id 1");
    reply = (redisReply*)redisCommand(context, "hmset P_ADs:0 fk_diagnose_id 1 dateOfStartTreatment 1 dateOfEndTreatment 1 fk_room_id 1 fk_patient_id 1 fk_doctor_id 1");
    reply = (redisReply*)redisCommand(context, "hmset records:0 fk_diagnose_id 1 dateOfVisit 1 fk_doctor_id 1 fk_P_AD_id 1");

    freeReplyObject(reply);
    return true;
}

void interface(redisContext*& context, vector<const char*> tables){
    bool isWork = true;
    while(isWork) {        
        cout << "Choose an option: (1)View&Open Table, (2)View Merged Table, (3)View All Tables, (4) Delete Data in All Tables, (5)Exit" << endl;
        switch (getInt(5, 1)) {
            case 1:
                viewTable(context, tables);
                break;
            case 2:
                viewMergedTable(context, tables);                    
                break;  
            case 3:
                showAllTables(context, tables);
                break;
            case 4:
                generationTables(context);
                break;
            case 5:
                rsSave(context);
                isWork = false;
                break;
            default:
                cout << "Error getInt()";
                isWork = false;
        }
    }
}

int main() {
    redisContext *context;
    if(rscon(context)){
        
        vector<const char*> tables = rsKEYS(context);
        sortVector(tables);
        if(tables.size() > 0){
            interface(context, tables);
        } else {          
            generationTables(context);
            tables = rsKEYS(context);
            sortVector(tables);
            cout << "Tables created" << endl;
            interface(context, tables);
        }

    }

    redisFree(context);
    return 0;
}

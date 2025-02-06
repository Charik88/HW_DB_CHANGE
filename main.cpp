#include "DB.h"

int main(int argc, char** argv)
{
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    try
    {
        std::string connection_string =
            "host=localhost "
            "port=5432 "
            "dbname = test "
            "user =  "
            "password = ";

        DB db(connection_string);
        db.createTables();

        db.addClient("Anastasia", "Charykova", "a.charykova@list.ru", "+79011555155");
        db.show();

        try
        {
            db.addPhone("Anastasia", "89011555155");
            db.show();
        }
        catch (const std::exception& ex)
        {
            std::cout << "Exception happened: " << ex.what() << std::endl;
        }

        try
        {
            db.updateClient("a.charykova@list.ru", "Anastasia", "Charykova", "a.charykova@list.ru");
            db.show();

        }
        catch (const std::exception& ex)
        {
            std::cout << "Exception happened: " << ex.what() << std::endl;
        }

        db.removePhone("a.charykova@list.ru", "+79011555155");

        db.removeClient("a.charykova@list.ru");

        std::vector<Client> foundClients = db.findClients("Anastasia");
        for (const auto& [name, surname, email, phones] : foundClients)
        {
            std::cout << "Found client: " << name << " " << surname << " " << email << " " << std::endl;
        }
        db.show();
    }
    catch (const std::exception& ex)
    {
        std::cout << "Exception happened: " << ex.what() << std::endl;
    }

    return 0;
};
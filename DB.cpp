#include "DB.h"
#include <stdexcept>
#include <iostream>
#include <pqxx/pqxx>

DB::DB(const std::string& connection) : connection_{ connection } {
    std::cout << "Успешно подключено к базе данных!" << std::endl;
}

void DB::createTables() {
    try {
        pqxx::transaction trn(connection_);
        trn.exec(R"(CREATE TABLE IF NOT EXISTS clients (id SERIAL PRIMARY KEY, first_name TEXT, last_name TEXT, UNIQUE(email TEXT)); 
                     CREATE TABLE IF NOT EXISTS phones (id SERIAL PRIMARY KEY, client_id INT REFERENCES clients(id), UNIQUE(phone TEXT));)");
        trn.commit();
        std::cout << "Созданы таблицы" << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при создании таблиц: " + std::string(e.what()));
    }
}

void DB::addClient(const std::string& first_name, const std::string& last_name, const std::string& email, const std::string& phone) {
    try {
        pqxx::transaction trn(connection_);
        connection_.set_client_encoding("UTF8");
        pqxx::result res = trn.exec_params("INSERT INTO clients (first_name, last_name, email) VALUES ($1, $2, $3) RETURNING id;", first_name, last_name, email);

        if (res.empty()) {
            throw std::runtime_error("Не удалось добавить клиента");
        }

        int client_id = res[0][0].as<int>();
        trn.exec_params("INSERT INTO phones(client_id, phone) VALUES ($1, $2);", client_id, phone);
        trn.commit();
        std::cout << "Клиент добавлен с идентификатором: " << client_id << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при добавлении клиента: " + std::string(e.what()));
    }
}

void DB::addPhone(const std::string& first_name, const std::string& phone) {
    try {
        pqxx::transaction trn(connection_);
        pqxx::result res = trn.exec_params("SELECT id, last_name FROM clients WHERE first_name = $1;", first_name);

        if (res.empty()) {
            throw std::runtime_error("Клиент не найден: " + first_name);
        }

        int client_id = res[0][0].as<int>();
        trn.exec_params("INSERT INTO phones (client_id, phone) VALUES ($1, $2);", client_id, phone);
        std::string last_name = res[0][1].as<std::string>();
        trn.commit();
        std::cout << "Телефон добавлен для клиента: " << client_id << " " << first_name << " " << last_name << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при добавлении телефона: " + std::string(e.what()));
    }
}

void DB::updateClient(const std::string& email, const std::string& newFirstName, const std::string& newLastName, const std::string& newEmail) {
    try {
        pqxx::transaction trn(connection_);
        pqxx::result res = trn.exec_params("SELECT id FROM clients WHERE email = $1;", email);

        if (res.empty()) {
            throw std::runtime_error("Клиент не найден: " + email);
        }

        int client_id = res[0][0].as<int>();
        trn.exec_params("UPDATE clients SET first_name = $1, last_name = $2, email = $3 WHERE id = $4;", newFirstName, newLastName, newEmail, client_id);
        trn.commit();
        std::cout << "Клиент обновлен: " << client_id << " " << newFirstName << " " << newLastName << " " << newEmail << std::endl;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Ошибка при обновлении клиента: " + std::string(e.what()));
    }
}

void DB::removePhone(const std::string& email, const std::string& phone) {
    try {
        pqxx::transaction trn(connection_);
        pqxx::result res = trn.exec_params("SELECT id, first_name, last_name FROM clients WHERE email = $1;", email);



use booking_vr;

CREATE TABLE GameSchedule (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    client_id BIGINT UNSIGNED NOT NULL,
    place_game VARCHAR(50) NOT NULL,
	name_game VARCHAR(50) NOT NULL,
	type_game VARCHAR(50) NOT NULL,
	date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) NOT NULL,
    who_reservation varchar(50) NOT NULL,
    book_status varchar(50) NOT NULL,
	date_add_book TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL
);

CREATE INDEX idx_game_schedule ON GameSchedule (name_game, date_game, time_game);


CREATE TABLE CombatSquad (
	id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    client_id BIGINT UNSIGNED NOT NULL,
    place_game VARCHAR(50) NOT NULL,
	name_game VARCHAR(50) NOT NULL,
	type_game VARCHAR(50) NOT NULL,
	date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 10 NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) NOT NULL,
    who_reservation varchar(50) NOT NULL,
    book_status varchar(50) NOT NULL,
	date_add_book TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT fk_shooter_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE 
);


CREATE TABLE MedievalJourney (
	id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    client_id BIGINT UNSIGNED NOT NULL,
    place_game VARCHAR(50) NOT NULL,
	name_game VARCHAR(50) NOT NULL,
	type_game VARCHAR(50) NOT NULL,
	date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 10 NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) NOT NULL,
    who_reservation varchar(50) NOT NULL,
    book_status varchar(50) NOT NULL,
	date_add_book TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT fk_quest_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE  
);

CREATE TABLE Cubes(
	id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    client_id BIGINT UNSIGNED NOT NULL,
    place_game VARCHAR(50) NOT NULL,
	name_game VARCHAR(50) NOT NULL,
	type_game VARCHAR(50) NOT NULL,
	date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 4 NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) NOT NULL,
    who_reservation varchar(50) NOT NULL,
    book_status varchar(50) NOT NULL,
	date_add_book TIMESTAMP DEFAULT CURRENT_TIMESTAMP NOT NULL,
    CONSTRAINT fk_cubes_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE  
);


CREATE TABLE Clients (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    email VARCHAR(100) NOT NULL,
    UNIQUE (phone, email)  
);



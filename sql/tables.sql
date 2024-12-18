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


CREATE TABLE OpenArenaButtonData (
    button_id varchar(50) NOT NULL,
    weekday INT NOT NULL,
    button_time varchar(30) NOT NULL,
    button_price INT NOT NULL,
    button_price_holidays INT NOT NULL,
    UNIQUE (button_id, weekday)
);


INSERT INTO OpenArenaButtonData (button_id, weekday, button_time, button_price, button_price_holidays) VALUES 
('button11', 0, '11:15 - 12:00', 1000, 1600),
('button12', 0, '12:15 - 13:00', 1000, 1600),
('button13', 0, '13:15 - 14:00', 1000, 1600),
('button14', 0, '14:15 - 15:00', 1000, 1600),
('button15', 0, '15:15 - 16:00', 1000, 1600),
('button16', 0, '16:15 - 17:00', 1000, 1600),
('button17', 0, '17:15 - 18:00', 1000, 1600),
('button18', 0, '18:15 - 19:00', 1000, 1600),
('button19', 0, '19:15 - 20:00', 1000, 1600),
('button20', 0, '20:15 - 21:00', 1000, 1600),
('button21', 0, '21:15 - 22:00', 1000, 1200),
('button22', 0, '22:15 - 23:00', 1000, 1200),


('button11', 1, '11:15 - 12:00', 1000, 1600),
('button12', 1, '12:15 - 13:00', 1000, 1600),
('button13', 1, '13:15 - 14:00', 1000, 1600),
('button14', 1, '14:15 - 15:00', 1000, 1600),
('button15', 1, '15:15 - 16:00', 1000, 1600),
('button16', 1, '16:15 - 17:00', 1000, 1600),
('button17', 1, '17:15 - 18:00', 1000, 1600),
('button18', 1, '18:15 - 19:00', 1000, 1600),
('button19', 1, '19:15 - 20:00', 1000, 1600),
('button20', 1, '20:15 - 21:00', 1000, 1600),
('button21', 1, '21:15 - 22:00', 1000, 1200),
('button22', 1, '22:15 - 23:00', 1000, 1200),


('button11', 2, '11:15 - 12:00', 1000, 1600),
('button12', 2, '12:15 - 13:00', 1000, 1600),
('button13', 2, '13:15 - 14:00', 1000, 1600),
('button14', 2, '14:15 - 15:00', 1000, 1600),
('button15', 2, '15:15 - 16:00', 1000, 1600),
('button16', 2, '16:15 - 17:00', 1000, 1600),
('button17', 2, '17:15 - 18:00', 1000, 1600),
('button18', 2, '18:15 - 19:00', 1000, 1600),
('button19', 2, '19:15 - 20:00', 1000, 1600),
('button20', 2, '20:15 - 21:00', 1000, 1600),
('button21', 2, '21:15 - 22:00', 1000, 1200),
('button22', 2, '22:15 - 23:00', 1000, 1200),


('button11', 3, '11:15 - 12:00', 1000, 1600),
('button12', 3, '12:15 - 13:00', 1000, 1600),
('button13', 3, '13:15 - 14:00', 1000, 1600),
('button14', 3, '14:15 - 15:00', 1000, 1600),
('button15', 3, '15:15 - 16:00', 1000, 1600),
('button16', 3, '16:15 - 17:00', 1000, 1600),
('button17', 3, '17:15 - 18:00', 1000, 1600),
('button18', 3, '18:15 - 19:00', 1000, 1600),
('button19', 3, '19:15 - 20:00', 1000, 1600),
('button20', 3, '20:15 - 21:00', 1000, 1600),
('button21', 3, '21:15 - 22:00', 1000, 1200),
('button22', 3, '22:15 - 23:00', 1000, 1200),


('button11', 4, '11:15 - 12:00', 1000, 1600),
('button12', 4, '12:15 - 13:00', 1000, 1600),
('button13', 4, '13:15 - 14:00', 1000, 1600),
('button14', 4, '14:15 - 15:00', 1000, 1600),
('button15', 4, '15:15 - 16:00', 1000, 1600),
('button16', 4, '16:15 - 17:00', 1000, 1600),
('button17', 4, '17:15 - 18:00', 1000, 1600),
('button18', 4, '18:15 - 19:00', 1600, 1600),
('button19', 4, '19:15 - 20:00', 1600, 1600),
('button20', 4, '20:15 - 21:00', 1600, 1600),
('button21', 4, '21:15 - 22:00', 1200, 1200),
('button22', 4, '22:15 - 23:00', 1200, 1200),


('button11', 5, '11:15 - 12:00', 1600, 1600),
('button12', 5, '12:15 - 13:00', 1600, 1600),
('button13', 5, '13:15 - 14:00', 1600, 1600),
('button14', 5, '14:15 - 15:00', 1600, 1600),
('button15', 5, '15:15 - 16:00', 1600, 1600),
('button16', 5, '16:15 - 17:00', 1600, 1600),
('button17', 5, '17:15 - 18:00', 1600, 1600),
('button18', 5, '18:15 - 19:00', 1600, 1600),
('button19', 5, '19:15 - 20:00', 1600, 1600),
('button20', 5, '20:15 - 21:00', 1600, 1600),
('button21', 5, '21:15 - 22:00', 1200, 1200),
('button22', 5, '22:15 - 23:00', 1200, 1200),


('button11', 6, '11:15 - 12:00', 1600, 1600),
('button12', 6, '12:15 - 13:00', 1600, 1600),
('button13', 6, '13:15 - 14:00', 1600, 1600),
('button14', 6, '14:15 - 15:00', 1600, 1600),
('button15', 6, '15:15 - 16:00', 1600, 1600),
('button16', 6, '16:15 - 17:00', 1600, 1600),
('button17', 6, '17:15 - 18:00', 1600, 1600),
('button18', 6, '18:15 - 19:00', 1600, 1600),
('button19', 6, '19:15 - 20:00', 1600, 1600),
('button20', 6, '20:15 - 21:00', 1600, 1600),
('button21', 6, '21:15 - 22:00', 1200, 1200),
('button22', 6, '22:15 - 23:00', 1200, 1200);





CREATE TABLE CloseArenaButtonData (
    button_id varchar(50) NOT NULL,
    weekday INT NOT NULL,
    button_time varchar(30) NOT NULL,
    button_price INT NOT NULL,
    button_price_holidays INT NOT NULL,
    UNIQUE (button_id, weekday)
);



INSERT INTO CloseArenaButtonData (button_id, weekday, button_time, button_price, button_price_holidays) VALUES 
('button11', 0, '11:15 - 12:00', 10000, 15000),
('button12', 0, '12:15 - 13:00', 10000, 15000),
('button13', 0, '13:15 - 14:00', 10000, 15000),
('button14', 0, '14:15 - 15:00', 10000, 15000),
('button15', 0, '15:15 - 16:00', 10000, 15000),
('button16', 0, '16:15 - 17:00', 10000, 15000),
('button17', 0, '17:15 - 18:00', 10000, 15000),
('button18', 0, '18:15 - 19:00', 10000, 15000),
('button19', 0, '19:15 - 20:00', 10000, 15000),
('button20', 0, '20:15 - 21:00', 10000, 15000),
('button21', 0, '21:15 - 22:00', 10000, 11000),
('button22', 0, '22:15 - 23:00', 10000, 11000),


('button11', 1, '11:15 - 12:00', 10000, 15000),
('button12', 1, '12:15 - 13:00', 10000, 15000),
('button13', 1, '13:15 - 14:00', 10000, 15000),
('button14', 1, '14:15 - 15:00', 10000, 15000),
('button15', 1, '15:15 - 16:00', 10000, 15000),
('button16', 1, '16:15 - 17:00', 10000, 15000),
('button17', 1, '17:15 - 18:00', 10000, 15000),
('button18', 1, '18:15 - 19:00', 10000, 15000),
('button19', 1, '19:15 - 20:00', 10000, 15000),
('button20', 1, '20:15 - 21:00', 10000, 15000),
('button21', 1, '21:15 - 22:00', 10000, 11000),
('button22', 1, '22:15 - 23:00', 10000, 11000),


('button11', 2, '11:15 - 12:00', 10000, 15000),
('button12', 2, '12:15 - 13:00', 10000, 15000),
('button13', 2, '13:15 - 14:00', 10000, 15000),
('button14', 2, '14:15 - 15:00', 10000, 15000),
('button15', 2, '15:15 - 16:00', 10000, 15000),
('button16', 2, '16:15 - 17:00', 10000, 15000),
('button17', 2, '17:15 - 18:00', 10000, 15000),
('button18', 2, '18:15 - 19:00', 10000, 15000),
('button19', 2, '19:15 - 20:00', 10000, 15000),
('button20', 2, '20:15 - 21:00', 10000, 15000),
('button21', 2, '21:15 - 22:00', 10000, 11000),
('button22', 2, '22:15 - 23:00', 10000, 11000),


('button11', 3, '11:15 - 12:00', 10000, 15000),
('button12', 3, '12:15 - 13:00', 10000, 15000),
('button13', 3, '13:15 - 14:00', 10000, 15000),
('button14', 3, '14:15 - 15:00', 10000, 15000),
('button15', 3, '15:15 - 16:00', 10000, 15000),
('button16', 3, '16:15 - 17:00', 10000, 15000),
('button17', 3, '17:15 - 18:00', 10000, 15000),
('button18', 3, '18:15 - 19:00', 10000, 15000),
('button19', 3, '19:15 - 20:00', 10000, 15000),
('button20', 3, '20:15 - 21:00', 10000, 15000),
('button21', 3, '21:15 - 22:00', 10000, 11000),
('button22', 3, '22:15 - 23:00', 10000, 11000),


('button11', 4, '11:15 - 12:00', 10000, 15000),
('button12', 4, '12:15 - 13:00', 10000, 15000),
('button13', 4, '13:15 - 14:00', 10000, 15000),
('button14', 4, '14:15 - 15:00', 10000, 15000),
('button15', 4, '15:15 - 16:00', 10000, 15000),
('button16', 4, '16:15 - 17:00', 10000, 15000),
('button17', 4, '17:15 - 18:00', 10000, 15000),
('button18', 4, '18:15 - 19:00', 15000, 15000),
('button19', 4, '19:15 - 20:00', 15000, 15000),
('button20', 4, '20:15 - 21:00', 15000, 15000),
('button21', 4, '21:15 - 22:00', 11000, 11000),
('button22', 4, '22:15 - 23:00', 11000, 11000),


('button11', 5, '11:15 - 12:00', 15000, 15000),
('button12', 5, '12:15 - 13:00', 15000, 15000),
('button13', 5, '13:15 - 14:00', 15000, 15000),
('button14', 5, '14:15 - 15:00', 15000, 15000),
('button15', 5, '15:15 - 16:00', 15000, 15000),
('button16', 5, '16:15 - 17:00', 15000, 15000),
('button17', 5, '17:15 - 18:00', 15000, 15000),
('button18', 5, '18:15 - 19:00', 15000, 15000),
('button19', 5, '19:15 - 20:00', 15000, 15000),
('button20', 5, '20:15 - 21:00', 15000, 15000),
('button21', 5, '21:15 - 22:00', 11000, 11000),
('button22', 5, '22:15 - 23:00', 11000, 11000),


('button11', 6, '11:15 - 12:00', 15000, 15000),
('button12', 6, '12:15 - 13:00', 15000, 15000),
('button13', 6, '13:15 - 14:00', 15000, 15000),
('button14', 6, '14:15 - 15:00', 15000, 15000),
('button15', 6, '15:15 - 16:00', 15000, 15000),
('button16', 6, '16:15 - 17:00', 15000, 15000),
('button17', 6, '17:15 - 18:00', 15000, 15000),
('button18', 6, '18:15 - 19:00', 15000, 15000),
('button19', 6, '19:15 - 20:00', 15000, 15000),
('button20', 6, '20:15 - 21:00', 15000, 15000),
('button21', 6, '21:15 - 22:00', 11000, 11000),
('button22', 6, '22:15 - 23:00', 11000, 11000);






CREATE TABLE CubesButtonData (
    button_id varchar(50) NOT NULL,
    weekday INT NOT NULL,
    button_time varchar(30) NOT NULL,
    button_price INT NOT NULL,
    button_price_holidays INT NOT NULL,
    UNIQUE (button_id, weekday)
);


INSERT INTO CubesButtonData (button_id, weekday, button_time, button_price, button_price_holidays) VALUES 
('button11', 0, '11:15 - 12:00', 1300, 1300),
('button12', 0, '12:15 - 13:00', 1300, 1300),
('button13', 0, '13:15 - 14:00', 1300, 1300),
('button14', 0, '14:15 - 15:00', 1300, 1300),
('button15', 0, '15:15 - 16:00', 1300, 1300),
('button16', 0, '16:15 - 17:00', 1300, 1300),
('button17', 0, '17:15 - 18:00', 1300, 1300),
('button18', 0, '18:15 - 19:00', 1300, 1300),
('button19', 0, '19:15 - 20:00', 1300, 1300),
('button20', 0, '20:15 - 21:00', 1300, 1300),
('button21', 0, '21:15 - 22:00', 1300, 1300),
('button22', 0, '22:15 - 23:00', 1300, 1300),


('button11', 1, '11:15 - 12:00', 1300, 1300),
('button12', 1, '12:15 - 13:00', 1300, 1300),
('button13', 1, '13:15 - 14:00', 1300, 1300),
('button14', 1, '14:15 - 15:00', 1300, 1300),
('button15', 1, '15:15 - 16:00', 1300, 1300),
('button16', 1, '16:15 - 17:00', 1300, 1300),
('button17', 1, '17:15 - 18:00', 1300, 1300),
('button18', 1, '18:15 - 19:00', 1300, 1300),
('button19', 1, '19:15 - 20:00', 1300, 1300),
('button20', 1, '20:15 - 21:00', 1300, 1300),
('button21', 1, '21:15 - 22:00', 1300, 1300),
('button22', 1, '22:15 - 23:00', 1300, 1300),


('button11', 2, '11:15 - 12:00', 1300, 1300),
('button12', 2, '12:15 - 13:00', 1300, 1300),
('button13', 2, '13:15 - 14:00', 1300, 1300),
('button14', 2, '14:15 - 15:00', 1300, 1300),
('button15', 2, '15:15 - 16:00', 1300, 1300),
('button16', 2, '16:15 - 17:00', 1300, 1300),
('button17', 2, '17:15 - 18:00', 1300, 1300),
('button18', 2, '18:15 - 19:00', 1300, 1300),
('button19', 2, '19:15 - 20:00', 1300, 1300),
('button20', 2, '20:15 - 21:00', 1300, 1300),
('button21', 2, '21:15 - 22:00', 1300, 1300),
('button22', 2, '22:15 - 23:00', 1300, 1300),


('button11', 3, '11:15 - 12:00', 1300, 1300),
('button12', 3, '12:15 - 13:00', 1300, 1300),
('button13', 3, '13:15 - 14:00', 1300, 1300),
('button14', 3, '14:15 - 15:00', 1300, 1300),
('button15', 3, '15:15 - 16:00', 1300, 1300),
('button16', 3, '16:15 - 17:00', 1300, 1300),
('button17', 3, '17:15 - 18:00', 1300, 1300),
('button18', 3, '18:15 - 19:00', 1300, 1300),
('button19', 3, '19:15 - 20:00', 1300, 1300),
('button20', 3, '20:15 - 21:00', 1300, 1300),
('button21', 3, '21:15 - 22:00', 1300, 1300),
('button22', 3, '22:15 - 23:00', 1300, 1300),


('button11', 4, '11:15 - 12:00', 1300, 1300),
('button12', 4, '12:15 - 13:00', 1300, 1300),
('button13', 4, '13:15 - 14:00', 1300, 1300),
('button14', 4, '14:15 - 15:00', 1300, 1300),
('button15', 4, '15:15 - 16:00', 1300, 1300),
('button16', 4, '16:15 - 17:00', 1300, 1300),
('button17', 4, '17:15 - 18:00', 1300, 1300),
('button18', 4, '18:15 - 19:00', 1300, 1300),
('button19', 4, '19:15 - 20:00', 1300, 1300),
('button20', 4, '20:15 - 21:00', 1300, 1300),
('button21', 4, '21:15 - 22:00', 1300, 1300),
('button22', 4, '22:15 - 23:00', 1300, 1300),


('button11', 5, '11:15 - 12:00', 1300, 1300),
('button12', 5, '12:15 - 13:00', 1300, 1300),
('button13', 5, '13:15 - 14:00', 1300, 1300),
('button14', 5, '14:15 - 15:00', 1300, 1300),
('button15', 5, '15:15 - 16:00', 1300, 1300),
('button16', 5, '16:15 - 17:00', 1300, 1300),
('button17', 5, '17:15 - 18:00', 1300, 1300),
('button18', 5, '18:15 - 19:00', 1300, 1300),
('button19', 5, '19:15 - 20:00', 1300, 1300),
('button20', 5, '20:15 - 21:00', 1300, 1300),
('button21', 5, '21:15 - 22:00', 1300, 1300),
('button22', 5, '22:15 - 23:00', 1300, 1300),


('button11', 6, '11:15 - 12:00', 1300, 1300),
('button12', 6, '12:15 - 13:00', 1300, 1300),
('button13', 6, '13:15 - 14:00', 1300, 1300),
('button14', 6, '14:15 - 15:00', 1300, 1300),
('button15', 6, '15:15 - 16:00', 1300, 1300),
('button16', 6, '16:15 - 17:00', 1300, 1300),
('button17', 6, '17:15 - 18:00', 1300, 1300),
('button18', 6, '18:15 - 19:00', 1300, 1300),
('button19', 6, '19:15 - 20:00', 1300, 1300),
('button20', 6, '20:15 - 21:00', 1300, 1300),
('button21', 6, '21:15 - 22:00', 1300, 1300),
('button22', 6, '22:15 - 23:00', 1300, 1300);






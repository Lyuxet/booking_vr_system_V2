use booking_vr;
-- Создание таблицы GameSchedule
CREATE TABLE GameSchedule (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    type_game VARCHAR(50) NOT NULL,
    name_game VARCHAR(50) NOT NULL,
    date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT NOT NULL,
    free_slots INT NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) DEFAULT 'Резерв' NOT NULL,
    UNIQUE (date_game, time_game, name_game)  -- Уникальная комбинация по дате, времени и названию игры
);

-- Создание индекса для внешнего ключа в таблице GameSchedule
CREATE INDEX idx_game_schedule ON GameSchedule (name_game, date_game, time_game);

-- Создание таблицы ArenaShooterStats
CREATE TABLE ArenaShooterStats (
    type_game VARCHAR(50) NOT NULL,
    name_game VARCHAR(50) DEFAULT 'ARENA SHOOTER' NOT NULL,
    date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 10 NOT NULL,
    free_slots INT DEFAULT 10 NOT NULL,
    price INT NOT NULL,
    comment_game VARCHAR(400) DEFAULT 'Резерв' NOT NULL,
    PRIMARY KEY (name_game, date_game, time_game),  -- Составной ключ
    CONSTRAINT fk_shooter_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE  -- Каскадное удаление для обеспечения целостности данных
);

-- Создание таблицы ArenaShooterStats
CREATE TABLE ArenaQuestStats (
    type_game VARCHAR(50) NOT NULL,
    name_game VARCHAR(50) DEFAULT 'ARENA QUEST' NOT NULL,
    date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 10 NOT NULL,
    free_slots INT DEFAULT 10 NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) DEFAULT 'Резерв' NOT NULL,
    PRIMARY KEY (name_game, date_game, time_game),  -- Составной ключ
    CONSTRAINT fk_quest_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE  -- Каскадное удаление для обеспечения целостности данных
);

CREATE TABLE Cubes(
	type_game VARCHAR(50) DEFAULT 'CUBES' NOT NULL,
    name_game VARCHAR(50) DEFAULT 'CUBES' NOT NULL,
    date_game DATE NOT NULL,
    time_game varchar(20) NOT NULL,
    players_count INT DEFAULT 0 NOT NULL,
    max_players INT DEFAULT 4 NOT NULL,
    free_slots INT DEFAULT 4 NOT NULL,
    price INT NOT NULL,
	comment_game VARCHAR(400) DEFAULT 'Резерв' NOT NULL,
    PRIMARY KEY (name_game, date_game, time_game),  -- Составной ключ
    CONSTRAINT fk_cubes_schedule FOREIGN KEY (name_game, date_game, time_game)
    REFERENCES GameSchedule(name_game, date_game, time_game)
    ON DELETE CASCADE  -- Каскадное удаление для обеспечения целостности данных


);

-- Созадние таблицы для персольных данных клиентов
CREATE TABLE Clients (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    first_name VARCHAR(100) NOT NULL,
    last_name VARCHAR(100) NOT NULL,
    phone VARCHAR(20) NOT NULL,
    email VARCHAR(100) NOT NULL,
    UNIQUE (phone, email)  -- уникальность телефона и почты для предотвращения дублирования
);

-- Связующая таблица между клиентами и бронированиями
CREATE TABLE Bookings (
    id BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY,
    client_id BIGINT UNSIGNED NOT NULL,
    game_id BIGINT UNSIGNED NOT NULL,
    booking_time TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (client_id) REFERENCES Clients(id) ON DELETE CASCADE,
    FOREIGN KEY (game_id) REFERENCES GameSchedule(id) ON DELETE CASCADE,
    UNIQUE (client_id, game_id)  -- Уникальная комбинация для исключения дублирования бронирований
);



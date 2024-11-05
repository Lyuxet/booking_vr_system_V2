-- Триггеры для шутера на арене____________________________________________________________________________________________
DELIMITER $$
CREATE TRIGGER check_and_update_CombatSquad
BEFORE INSERT ON CombatSquad
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
END$$
DELIMITER ;

DELIMITER $$
CREATE TRIGGER check_and_insert_CombatSquad_schedule
BEFORE INSERT ON CombatSquad
FOR EACH ROW
BEGIN
    -- Проверяем, занято ли время для другой игры в это же время
    IF EXISTS (
        SELECT 1
        FROM GameSchedule
        WHERE date_game = NEW.date_game
          AND time_game = NEW.time_game
          AND name_game != NEW.name_game
          AND type_game = NEW.type_game
    ) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
    -- Проверяем, занято ли это время для текущей игры
    ELSE
        -- Если время свободно, добавляем запись в таблицу расписания
        INSERT INTO GameSchedule (client_id, place_game, type_game, name_game, date_game, time_game, players_count, max_players, price, comment_game, who_reservation, book_status)
        VALUES (NEW.client_id, NEW.place_game, NEW.type_game, NEW.name_game, NEW.date_game, NEW.time_game, NEW.players_count, NEW.max_players, NEW.price, NEW.comment_game, NEW.who_reservation, NEW.book_status);
    END IF;
END$$

DELIMITER ;


DELIMITER $$

CREATE TRIGGER check_and_update_CombatSquad_before_update
BEFORE UPDATE ON CombatSquad
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
    -- Проверяем, изменяются ли ключевые поля (date_game, time_game, name_game)
    IF NEW.date_game != OLD.date_game OR NEW.time_game != OLD.time_game OR NEW.name_game != OLD.name_game THEN
        
        -- Проверяем, занято ли время для другой игры в это же время
        IF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game != NEW.name_game
              AND type_game = NEW.type_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
        
        -- Проверяем, занято ли это время для текущей игры
        ELSEIF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game = NEW.name_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot for this game is already taken.';
        END IF;

        -- Обновляем запись в таблице GameSchedule с новыми ключевыми полями
        UPDATE GameSchedule 
        SET place_game = NEW.place_game,
			type_game = NEW.type_game, 
            name_game = NEW.name_game,
            date_game = NEW.date_game,
            time_game = NEW.time_game,
            players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;	

    ELSE
        -- Если ключевые поля не меняются, просто обновляем неключевые поля
        UPDATE GameSchedule 
        SET players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;
    END IF;

END$$

DELIMITER ;


DELIMITER $$
CREATE TRIGGER 	delete_CombatSquadr_schedule
BEFORE DELETE ON CombatSquad
FOR EACH ROW
BEGIN
	DELETE FROM GameSchedule
    WHERE name_game = OLD.name_game
    AND date_game = OLD.date_game
	AND time_game = OLD.time_game;
END$$
DELIMITER ;

-- Тригреры для квеста на арене_____________________________________________________________________________________________________
DELIMITER $$
CREATE TRIGGER check_and_update_MedievalJourney
BEFORE INSERT ON MedievalJourney
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
END$$
DELIMITER ;


DELIMITER $$
CREATE TRIGGER check_and_insert_MedievalJourney_schedule
BEFORE INSERT ON MedievalJourney
FOR EACH ROW
BEGIN
    -- Проверяем, занято ли время для другой игры в это же время
    IF EXISTS (
        SELECT 1
        FROM GameSchedule
        WHERE date_game = NEW.date_game
          AND time_game = NEW.time_game
          AND name_game != NEW.name_game
          AND type_game = NEW.type_game
    ) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
    -- Проверяем, занято ли это время для текущей игры
    ELSE
        -- Если время свободно, добавляем запись в таблицу расписания
        INSERT INTO GameSchedule (client_id, place_game, type_game, name_game, date_game, time_game, players_count, max_players, price, comment_game, who_reservation, book_status)
        VALUES (NEW.client_id, NEW.place_game, NEW.type_game, NEW.name_game, NEW.date_game, NEW.time_game, NEW.players_count, NEW.max_players, NEW.price, NEW.comment_game, NEW.who_reservation, NEW.book_status);
    END IF;
END$$
DELIMITER ;


DELIMITER $$

CREATE TRIGGER check_and_update_MedievalJourney_before_update
BEFORE UPDATE ON MedievalJourney
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
    -- Проверяем, изменяются ли ключевые поля (date_game, time_game, name_game)
    IF NEW.date_game != OLD.date_game OR NEW.time_game != OLD.time_game OR NEW.name_game != OLD.name_game THEN
        
        -- Проверяем, занято ли время для другой игры в это же время
        IF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game != NEW.name_game
              AND type_game = NEW.type_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
        
        -- Проверяем, занято ли это время для текущей игры
        ELSEIF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game = NEW.name_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot for this game is already taken.';
        END IF;

        -- Обновляем запись в таблице GameSchedule с новыми ключевыми полями
        UPDATE GameSchedule 
        SET place_game = NEW.place_game,
			type_game = NEW.type_game, 
            name_game = NEW.name_game,
            date_game = NEW.date_game,
            time_game = NEW.time_game,
            players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;

    ELSE
        -- Если ключевые поля не меняются, просто обновляем неключевые поля
        UPDATE GameSchedule 
        SET players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;
    END IF;

END$$

DELIMITER ;


DELIMITER $$
CREATE TRIGGER 	delete_MedievalJourney_schedule
BEFORE DELETE ON MedievalJourney
FOR EACH ROW
BEGIN
	DELETE FROM GameSchedule
    WHERE name_game = OLD.name_game
    AND date_game = OLD.date_game
	AND time_game = OLD.time_game;
END$$
DELIMITER ;

-- Триггеры для кубов__________________________________________________________________________________________________________________________
DELIMITER $$
CREATE TRIGGER check_and_update_cubes_stats
BEFORE INSERT ON Cubes
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
END$$
DELIMITER ;


DELIMITER $$
CREATE TRIGGER check_and_insert_cubes_schedule
BEFORE INSERT ON Cubes
FOR EACH ROW
BEGIN
    -- Проверяем, занято ли время для другой игры в это же время
    IF EXISTS (
        SELECT 1
        FROM GameSchedule
        WHERE date_game = NEW.date_game
          AND time_game = NEW.time_game
          AND name_game != NEW.name_game
          AND type_game = NEW.type_game
    ) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
    -- Проверяем, занято ли это время для текущей игры
    ELSE
        -- Если время свободно, добавляем запись в таблицу расписания
        INSERT INTO GameSchedule (client_id, place_game, type_game, name_game, date_game, time_game, players_count, max_players, price, comment_game, who_reservation, book_status)
        VALUES (NEW.client_id, NEW.place_game, NEW.type_game, NEW.name_game, NEW.date_game, NEW.time_game, NEW.players_count, NEW.max_players, NEW.price, NEW.comment_game, NEW.who_reservation, NEW.book_status);
    END IF;
END$$
DELIMITER ;

DELIMITER $$

CREATE TRIGGER check_and_update_cubes_stats_before_update
BEFORE UPDATE ON Cubes
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;
    -- Проверяем, изменяются ли ключевые поля (date_game, time_game, name_game)
    IF NEW.date_game != OLD.date_game OR NEW.time_game != OLD.time_game OR NEW.name_game != OLD.name_game THEN
        
        -- Проверяем, занято ли время для другой игры в это же время
        IF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game != NEW.name_game
              AND place_game = NEW.place_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot is already taken by another game.';
        
        -- Проверяем, занято ли это время для текущей игры
        ELSEIF EXISTS (
            SELECT 1
            FROM GameSchedule
            WHERE date_game = NEW.date_game
              AND time_game = NEW.time_game
              AND name_game = NEW.name_game
        ) THEN
            SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot for this game is already taken.';
        END IF;

        -- Обновляем запись в таблице GameSchedule с новыми ключевыми полями
        UPDATE GameSchedule 
        SET 
			place_game = NEW.place_game, 
            name_game = NEW.name_game,
            date_game = NEW.date_game,
            time_game = NEW.time_game,
            players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;

    ELSE
        -- Если ключевые поля не меняются, просто обновляем неключевые поля
        UPDATE GameSchedule 
        SET players_count = NEW.players_count,
            max_players = NEW.max_players, 
            price = NEW.price,
            comment_game = NEW.comment_game,
            book_status = NEW.book_status
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;
    END IF;

END$$

DELIMITER ;

DELIMITER $$
CREATE TRIGGER 	delete_cubes_schedule
BEFORE DELETE ON Cubes
FOR EACH ROW
BEGIN
	DELETE FROM GameSchedule
    WHERE name_game = OLD.name_game
    AND date_game = OLD.date_game
	AND time_game = OLD.time_game;
END$$
DELIMITER ;







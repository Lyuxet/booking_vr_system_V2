-- Триггеры для шутера на арене____________________________________________________________________________________________
DELIMITER $$
CREATE TRIGGER check_and_update_shooter_stats
BEFORE INSERT ON ArenaShooterStats
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;

    -- Рассчитываем количество свободных мест
    SET NEW.free_slots = NEW.max_players - NEW.players_count;
END$$
DELIMITER ;

DELIMITER $$
CREATE TRIGGER check_and_insert_shooter_schedule
BEFORE INSERT ON ArenaShooterStats
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
    ELSEIF EXISTS (
        SELECT 1
        FROM GameSchedule
        WHERE date_game = NEW.date_game
          AND time_game = NEW.time_game
          AND name_game = NEW.name_game
    ) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot for this game is already taken.';
    ELSE
        -- Если время свободно, добавляем запись в таблицу расписания
        INSERT INTO GameSchedule (type_game, name_game, date_game, time_game, players_count, max_players, free_slots, comment_game)
        VALUES (NEW.type_game, NEW.name_game, NEW.date_game, NEW.time_game, NEW.players_count, NEW.max_players, NEW.free_slots, NEW.comment_game);
    END IF;
END$$

DELIMITER ;


DELIMITER $$

CREATE TRIGGER check_and_update_shooter_stats_before_update
BEFORE UPDATE ON ArenaShooterStats
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;

    -- Рассчитываем количество свободных мест
    SET NEW.free_slots = NEW.max_players - NEW.players_count;

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
        SET type_game = NEW.type_game, 
            name_game = NEW.name_game,
            date_game = NEW.date_game,
            time_game = NEW.time_game,
            players_count = NEW.players_count,
            max_players = NEW.max_players, 
            free_slots = NEW.free_slots, 
            comment_game = NEW.comment_game
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;

    ELSE
        -- Если ключевые поля не меняются, просто обновляем неключевые поля
        UPDATE GameSchedule 
        SET players_count = NEW.players_count,
            max_players = NEW.max_players, 
            free_slots = NEW.free_slots, 
            comment_game = NEW.comment_game
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;
    END IF;

END$$

DELIMITER ;


DELIMITER $$
CREATE TRIGGER 	delete_shooter_schedule
BEFORE DELETE ON ArenaShooterStats
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
CREATE TRIGGER check_and_update_quest_stats
BEFORE INSERT ON ArenaQuestStats
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;

    -- Рассчитываем количество свободных мест
    SET NEW.free_slots = NEW.max_players - NEW.players_count;
END$$
DELIMITER ;


DELIMITER $$
CREATE TRIGGER check_and_insert_quest_schedule
BEFORE INSERT ON ArenaQuestStats
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
    ELSEIF EXISTS (
        SELECT 1
        FROM GameSchedule
        WHERE date_game = NEW.date_game
          AND time_game = NEW.time_game
          AND name_game = NEW.name_game
    ) THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Time slot for this game is already taken.';
    ELSE
        -- Если время свободно, добавляем запись в таблицу расписания
        INSERT INTO GameSchedule (type_game, name_game, date_game, time_game, players_count, max_players, free_slots, comment_game)
        VALUES (NEW.type_game, NEW.name_game, NEW.date_game, NEW.time_game, NEW.players_count, NEW.max_players, NEW.free_slots, NEW.comment_game);
    END IF;
END$$
DELIMITER ;


DELIMITER $$

CREATE TRIGGER check_and_update_quest_stats_before_update
BEFORE UPDATE ON ArenaQuestStats
FOR EACH ROW
BEGIN
    -- Проверяем, что количество игроков не превышает максимальное количество
    IF NEW.players_count > NEW.max_players THEN
        SIGNAL SQLSTATE '45000' SET MESSAGE_TEXT = 'Players count exceeds the maximum number of players.';
    END IF;

    -- Рассчитываем количество свободных мест
    SET NEW.free_slots = NEW.max_players - NEW.players_count;

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
        SET type_game = NEW.type_game, 
            name_game = NEW.name_game,
            date_game = NEW.date_game,
            time_game = NEW.time_game,
            players_count = NEW.players_count,
            max_players = NEW.max_players, 
            free_slots = NEW.free_slots, 
            comment_game = NEW.comment_game
        WHERE name_game = OLD.name_game 
          AND date_game = OLD.date_game 
          AND time_game = OLD.time_game;

    ELSE
        -- Если ключевые поля не меняются, просто обновляем неключевые поля
        UPDATE GameSchedule 
        SET players_count = NEW.players_count,
            max_players = NEW.max_players, 
            free_slots = NEW.free_slots, 
            comment_game = NEW.comment_game
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







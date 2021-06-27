-- SET foreign_key_checks = 0;

DROP TABLE IF EXISTS pictures;
DROP TABLE IF EXISTS goods_sellouts;
DROP TABLE IF EXISTS goods_logs;
DROP TABLE IF EXISTS user_permissions;
DROP TABLE IF EXISTS discounts;
DROP TABLE IF EXISTS goods;
DROP TABLE IF EXISTS users;
DROP TABLE IF EXISTS categories;

-- SET foreign_key_checks = 1;

-- 商品分类 (categories)
CREATE TABLE categories (
    cid INT NOT NULL AUTO_INCREMENT,
    name NCHAR(100),
    number INT,

    PRIMARY KEY(cid)
);

-- 用户信息 (users)
CREATE TABLE users (
    uid INT NOT NULL AUTO_INCREMENT,
    username NCHAR(50),
    password CHAR(32),
    role INT DEFAULT 2,
    phone CHAR(20),
    email NCHAR(100),
    last_login DATETIME,

    PRIMARY KEY(uid)
);

-- 商品信息表 (goods)
CREATE TABLE goods (
    gid INT NOT NULL AUTO_INCREMENT,
    gno CHAR(20),
    cid INT,
    uid INT,
    sku NCHAR(100),
    title NCHAR(50),
    subtitle NCHAR(200),
    inventory INT,
    restock_value FLOAT,
    selling_value FLOAT,
    length FLOAT,
    width FLOAT,
    height FLOAT,
    WEIGHT FLOAT,
    color NCHAR(20),

    PRIMARY KEY(gid),
    FOREIGN KEY(cid) REFERENCES categories(cid),
    FOREIGN KEY(uid) REFERENCES users(uid)
);

-- 商品图片 (pictures)
CREATE TABLE pictures (
    pid INT NOT NULL AUTO_INCREMENT,
    gid INT,
    pictures TEXT,

    PRIMARY KEY(pid),
    FOREIGN KEY(gid) REFERENCES goods(gid)
);

-- 商品卖出信息表 (goods_sellouts)
CREATE TABLE goods_sellouts (
    sid INT NOT NULL AUTO_INCREMENT,
    gid INT,
    uid INT,
    number FLOAT,
    price FLOAT,
    customer_name NCHAR(20),
    customer_contact NCHAR(20),
    sell_time DATETIME,

    PRIMARY KEY(sid),
    FOREIGN KEY(gid) REFERENCES goods(gid),
    FOREIGN KEY(uid) REFERENCES users(uid)
);

-- 商品变动信息表 (good_logs)
CREATE TABLE goods_logs (
    log_id INT NOT NULL AUTO_INCREMENT,
    gid INT,
    uid INT,
    type INT,
    number FLOAT,
    time DATETIME,

    PRIMARY KEY(log_id),
    FOREIGN KEY(gid) REFERENCES goods(gid),
    FOREIGN KEY(uid) REFERENCES users(uid)
);

-- 用户权限表 (user_permissions)
CREATE TABLE user_permissions (
    pid INT NOT NULL AUTO_INCREMENT,
    uid INT,
    type CHAR(50),
    domain CHAR(50),

    PRIMARY KEY(pid),
    FOREIGN KEY(uid) REFERENCES users(uid)
);

-- 商品折扣策略
CREATE TABLE discounts (
    did INT NOT NULL AUTO_INCREMENT,
    gid INT,
    uid INT,
    start_time DATETIME,
    end_time DATETIME,
    amount FLOAT,
    discount FLOAT,

    PRIMARY KEY(did),
    FOREIGN KEY(gid) REFERENCES goods(gid),
    FOREIGN KEY(uid) REFERENCES users(uid)
);

-- 更新库存量的触发器
DROP TRIGGER IF EXISTS increaseInventory;
CREATE TRIGGER increaseInventory AFTER INSERT ON goods_logs FOR EACH ROW
    UPDATE goods 
    SET inventory = inventory + NEW.number
    WHERE gid = NEW.gid;

DROP TRIGGER IF EXISTS decreaseInventory;
CREATE TRIGGER decreaseInventory AFTER INSERT ON goods_sellouts FOR EACH ROW
    UPDATE goods
    SET inventory = inventory - NEW.number
    WHERE gid = NEW.gid;

-- 更新商品分类计数器的触发器
DROP TRIGGER IF EXISTS categoriesNumberUpdater;
CREATE TRIGGER categoriesNumberUpdater AFTER UPDATE ON goods FOR EACH ROW
    UPDATE categories
    SET number = number - OLD.inventory + NEW.inventory
    WHERE cid = NEW.cid;

-- 创建默认管理员用户, 用户名 admin, 密码 admin
INSERT INTO users (username, password, role) VALUES ('admin', '21232f297a57a5a743894a0e4a801fc3', 0);

-- 创建默认分类
INSERT INTO categories (cid, name, number) VALUES (0, '未分类', 0);
CREATE DATABASE IF NOT EXISTS cloud_drive DEFAULT CHARSET=utf8mb4;
USE cloud_drive;

-- ==========================================
-- 1. 用户表 (users)
-- ==========================================
CREATE TABLE IF NOT EXISTS users (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    username    VARCHAR(64)  NOT NULL UNIQUE,
    password    VARCHAR(128) NOT NULL,             -- crypt 加密后
    salt        VARCHAR(32)  NOT NULL,             -- 盐值
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ==========================================
-- 2. 真实文件级元数据 (real_files) -> 类似 inode
-- ==========================================
CREATE TABLE IF NOT EXISTS real_files (
    file_md5    VARCHAR(64) PRIMARY KEY,           -- 整个文件的MD5作为主键
    file_size   BIGINT NOT NULL,                   -- 文件总大小
    chunk_count INT NOT NULL,                      -- 分了多少个块
    ref_count   INT DEFAULT 0,                     -- 引用计数（被多少个虚拟文件引用）
    status      TINYINT NOT NULL DEFAULT 0,        -- 0=上传中  1=已完成
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- ==========================================
-- 3. 真实块数据 (chunks)
-- ==========================================
CREATE TABLE IF NOT EXISTS chunks (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    file_md5    VARCHAR(64) NOT NULL,              -- 属于哪个文件
    chunk_index INT NOT NULL,                      -- 块序号（0, 1, 2...）
    chunk_md5   VARCHAR(64) NOT NULL,              -- 这一小块的MD5（用于单块校验）
    chunk_size  INT NOT NULL,                      -- 块大小（最后一个块可能不足1MB）
    real_path   VARCHAR(512) NOT NULL,             -- 这一块在服务器上的真实物理存储路径
    FOREIGN KEY (file_md5) REFERENCES real_files(file_md5),
    UNIQUE KEY  idx_file_chunk (file_md5, chunk_index) -- 同一个文件的同一块只有一条
);

-- ==========================================
-- 4. 虚拟文件/目录项 (virtual_files) -> 类似 dentry
-- ==========================================
CREATE TABLE IF NOT EXISTS virtual_files (
    id              INT AUTO_INCREMENT PRIMARY KEY,
    user_id         INT NOT NULL,
    parent_id       INT NOT NULL DEFAULT 0,            -- 父目录 ID，0 表示根目录
    filename        VARCHAR(255) NOT NULL,             -- 文件或目录的名称
    is_dir          TINYINT DEFAULT 0,                 -- 0=文件，1=目录
    file_md5        VARCHAR(64),                       -- 指向的真实文件MD5 (目录则为空)
    created_at      TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id),
    
    -- 约束：同一个用户下、同一个父目录下，文件名不能重复
    UNIQUE KEY  idx_path (user_id, parent_id, filename)
);
-- 索引：加速 ls 查询
CREATE INDEX idx_ls ON virtual_files(user_id, parent_id);
-- 幻顾

CREATE TABLE IF NOT EXISTS t_xreader_book_info (
    `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    `bid` char(32) NOT NULL DEFAULT '' COMMENT 'book id',
    `btitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'book title',
    `bsubtitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'book subtitle',
    `author` char(128) NOT NULL DEFAULT '' COMMENT 'author',
    `category` varchar(64) NOT NULL DEFAULT '' COMMENT 'category',
    `cover` varchar(255) NOT NULL DEFAULT '' COMMENT 'cover',
    `introduction` text COMMENT 'introduction',
    `status` int(8) NOT NULL DEFAULT 0 COMMENT '状态, 0:未完结 1:完结 2:烂尾',
    `word_cnt` int(11) NOT NULL DEFAULT 0 COMMENT 'words count',
    `chapter_cnt` int(11) NOT NULL DEFAULT 0 COMMENT 'chapters count',
    `chapter_latest` int(11) NOT NULL DEFAULT 0 COMMENT 'chapter id latest',
    `ts_start`  datetime NOT NULL DEFAULT '1000-01-01 00:00:00' COMMENT '开始创作时间',
    `ts_latest` datetime NOT NULL DEFAULT '1000-01-01 00:00:00' COMMENT '最后更新时间',
    `create_ts` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP COMMENT '记录创建时间',
    `modify_ts` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录最后修改时间',
    PRIMARY KEY (`id`),
    UNIQUE KEY `uniq_xreader_bid` (`bid`),
    KEY `index_xreader_na_au` (`btitle`, `author`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='book信息';

CREATE TABLE IF NOT EXISTS t_xreader_book_volumes (
    `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    `bid` char(32) NOT NULL DEFAULT '' COMMENT 'book id',
    `vid` int(11) NOT NULL DEFAULT 0 COMMENT 'volume id',
    `vtitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'volume title',
    `vsubtitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'volume subtitle',
    `modify_ts` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录最后修改时间',
    PRIMARY KEY (`id`),
    UNIQUE KEY `uniq_xreader_bvid` (`bid`,`vid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='book卷信息';

CREATE TABLE IF NOT EXISTS t_xreader_book_chapters (
    `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '自增ID',
    `bid` char(32) NOT NULL DEFAULT '' COMMENT 'book id',
    `vid` int(11) NOT NULL DEFAULT 0 COMMENT 'volume id',
    `cid` int(11) NOT NULL DEFAULT 0 COMMENT 'chapter id',
    `ctitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'chapter title',
    `csubtitle` varchar(128) NOT NULL DEFAULT '' COMMENT 'chapter subtitle',
    `content`  varchar(255) NOT NULL DEFAULT '' COMMENT 'chapter content',
    `word_cnt` int(11) NOT NULL DEFAULT 0 COMMENT 'words count of current chapter',
    `modify_ts` datetime NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '记录最后修改时间',
    PRIMARY KEY (`id`),
    UNIQUE KEY `uniq_xreader_bvcid` (`bid`,`vid`,`cid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COMMENT='book章节信息';

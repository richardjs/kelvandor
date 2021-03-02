import {RES_VACANT, RES_BLUE, RES_GREEN, RES_RED, RES_YELLOW, SIDE_NONE, SIDE_1, SIDE_2} from '../core/constants.js';

export const SIZE_TILE = 100;
export const SIZE_TILE_DOT = 10;
export const SIZE_NODE = 15;
export const SIZE_ROAD_W = 53;
export const SIZE_ROAD_H = 20;
export const SIZE_ROAD_EMPTY_W = 53;
export const SIZE_ROAD_EMPTY_H = 10;
export const SIZE_TRADE_W = 100;
export const SIZE_TRADE_H = 50;


export const UNIT_TILE = SIZE_TILE/2;
export const UNIT_NODE = SIZE_TILE/2;
export const UNIT_ROAD = SIZE_TILE/2;
export const UNIT_TRADE = SIZE_TRADE_W+10;

export const COUNT_TRADE = 3;

export const RES_COLORS = new Array(5);
RES_COLORS[RES_VACANT] = '#c0c0c0';
RES_COLORS[RES_BLUE] = '#0000ff';
RES_COLORS[RES_GREEN] = '#00ff00';
RES_COLORS[RES_RED] = '#ff0000';
RES_COLORS[RES_YELLOW] = '#ffff00';

export const RES_COLORS_HOVER = new Array(5);
RES_COLORS_HOVER[RES_VACANT] = '#aaaaaa';
RES_COLORS_HOVER[RES_BLUE] = '#0000aa';
RES_COLORS_HOVER[RES_GREEN] = '#00aa00';
RES_COLORS_HOVER[RES_RED] = '#aa0000';
RES_COLORS_HOVER[RES_YELLOW] = '#aaaa00';

export const SIDE_COLORS = new Array(3);
SIDE_COLORS[SIDE_NONE] = '#aaaaaa';
SIDE_COLORS[SIDE_1] = '#ffa500';
SIDE_COLORS[SIDE_2] = '#9e7bb5';

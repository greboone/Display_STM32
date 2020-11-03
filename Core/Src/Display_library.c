/*
 * Display_library.c
 *
 *  Created on: Nov 1, 2020
 *      Author: gutoe
 */

#include "Display_library.h"

// Caracteres com acentos, á, é, ç, ã, õ
unsigned char caracter1[8] = {0x02, 0x04, 0x0E, 0x01, 0x0F, 0x11, 0x0F, 0x00};
unsigned char caracter2[8] = {0x02, 0x04, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00};
unsigned char caracter3[8] = {0x00, 0x0F, 0x10, 0x10, 0x0F, 0x02, 0x0E, 0x00};
unsigned char caracter4[8] = {0x0D, 0x12, 0x06, 0x01, 0x0F, 0x11, 0x0F, 0x00};
unsigned char caracter5[8] = {0x0D, 0x12, 0x00, 0x0E, 0x11, 0x11, 0x0E, 0x00};

/**
 * @brief Pulso de alta para baixa em Ativar pino para travar dados
 *
 * @retval None
 */
void lcd_time() {

    HAL_GPIO_WritePin(LCD_PORT_CTRL, LCD_EN, 1); // => E = 1
    HAL_Delay(5);

    HAL_GPIO_WritePin(LCD_PORT_CTRL, LCD_EN, 0); // => E = 0
}

/**
 * @brief Inicializa o Display LCD
 * Inicializa o display no modo 4 bits, com cursor off, cursor anda para direita e limpa o lcd
 *
 * @retval None
 */
void lcd_init() {
    HAL_Delay(15); /* 15ms,16x2 LCD Power on delay */
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK, 0);

    // uses 4 bits
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & 0x20, 1);
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (~0x20), 0);
    lcd_time();

    lcd_cmd(0x28); /* usa 2 linhas para inicializar a matriz 5*7 do LCD */
    lcd_cmd(0x0c); /* display ligado cursor desligado */
    lcd_cmd(0x06); /* increment cursor (shift do cursor para direita) */
    lcd_cmd(0x01); /* limpa a tela */
}

/**
 * @brief Limpa a tela do display LCD e retorna o cursor para a posição inicial
 *
 * @retval None
 */
void lcd_clear() { lcd_cmd(0x01); }

/**
 * @brief Manda comandos para o LCD
 *
 * @param cmd Será o comando a ser mandado para o LCD
 * @retval None
 */
void lcd_cmd(char cmd) {
    HAL_GPIO_WritePin(LCD_PORT_CTRL, LCD_RS, 0);

    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & cmd, 1);
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (~cmd), 0);
    lcd_time();

    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (cmd << 4), 1);
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (~(cmd << 4)), 0);
    lcd_time();
}

/**
 * @brief Envia dados para o LCD
 *
 * @param data Este são os dados a serem enviados para o LCD
 * @retval None
 */
void lcd_char(uint32_t data) {
    if ((data >> 8) == 0xC3) {
        switch (data & 0xFF) {
            case 0xA1: data = 0; break;
            case 0xA9: data = 1; break;
            case 0xA7: data = 2; break;
            case 0xA3: data = 3; break;
            case 0xB5: data = 4; break;
            default: break;
        }
    }

    HAL_GPIO_WritePin(LCD_PORT_CTRL, LCD_RS, 1);

    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (data), 1);
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (~data), 0);
    lcd_time();

    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (data << 4), 1);
    HAL_GPIO_WritePin(LCD_PORT_DATA, LCD_D_BIT_MASK & (~(data << 4)), 0);
    lcd_time();
}

/**
 * @brief Evia uma string para o LCD
 *
 * @param msg Esta é a string a ser enviada para o LCD
 * @retval None
 */
void lcd_string(const char *msg) {
    while ((*msg) != 0) {
        if (*msg == 0xC3) {
            switch (*(++msg)) {
                case 0xA1: lcd_char(0); break;
                case 0xA9: lcd_char(1); break;
                case 0xA7: lcd_char(2); break;
                case 0xA3: lcd_char(3); break;
                case 0xB5: lcd_char(4); break;
                default: continue;
            }
        } else {
            lcd_char(*msg);
        }
        msg++;
    }
}

/**
 * @brief Envia uma string em uma posição especifica do LCD
 *
 * @param pos A posição no LCD onde a string deve ser exibida
 * @param msg A string a ser enviada ao LCD
 * @retval None
 */
void lcd_string_pos(char pos, const char *msg) {
    lcd_cmd(pos);
    lcd_string(msg);
}

/**
 * @brief Defina um caractere personalizado no LCD
 *
 * @param loc A localização do personagem deve ser definida [0 - 8]
 * @param msg Vetor com o caracter personalizado
 * @retval None
 */
void lcd_custom_char(unsigned char loc, unsigned char *msg) {
    unsigned char i;
    if (loc < 8) {
        lcd_cmd(0x40 +
                (loc * 8)); /* O comando 0x40 em diante força o dispositivo a apontar o endereço CGRAM */
        for (i = 0; i < 8; i++)
            lcd_char(msg[i]);
    }
}

/**
 * @brief Carregando os caracteres personalizados padrão
 *
 * @retval None
 */
void lcd_default_custom_char() {
    lcd_custom_char(0, caracter1); /* Grave o caractere personalizado no local de memória CGRAM 0x0X */
    lcd_custom_char(1, caracter2);
    lcd_custom_char(2, caracter3);
    lcd_custom_char(3, caracter4);
    lcd_custom_char(4, caracter5);
}


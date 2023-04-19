#include "LTG_lib_NMEA2k.hpp"



/**
 * @brief PGN 128259 | fonction qui permet de traiter les valeurs d'entrée (numérique) en binaire et placement dans un tableau puis envoie au MCP2515
 * 
 * @param SRC_adresse_envoye            Adresse de notre module sur le bus NMEA2000
 * @param Sequence_ID                   Donnée (voir datasheet)
 * @param Speed_Water_Referenced        Donnée (voir datasheet)
 * @param Speed_Ground_Referenced       Donnée (voir datasheet)
 * @param Speed_Water_Referenced_Type   Donnée (voir datasheet)
 * @param Speed_Direction               Donnée (voir datasheet)
 * @param MCP2515_1                     
 * @return uint8_t                      Return l'état du MCP
 */
uint8_t NMEA2k_Envoi_PGN_128259(uint8_t SRC_adresse_envoye, uint8_t Sequence_ID, float Speed_Water_Referenced, float Speed_Ground_Referenced, uint8_t Speed_Water_Referenced_Type, uint8_t Speed_Direction, MCP2515_LTG_class& MCP2515_1)
{
  // création des variables
  uint64_t message = 0;                               // variable de 64 bits qui permet d'organiser les bits dans le bon sens
  uint8_t mcp_message[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // tableau à envoyer vers le MCP
  uint16_t Speed_Water_Referenced_msg = 0;            // variable à mettre dans le message
  uint16_t Speed_Ground_Referenced_msg = 0;           // variable à mettre dans le message

  uint8_t RTR = 0;         //  type de réponse attendue
  uint8_t Prio = 0;        // prioritée
  uint32_t PGN_envoye = 0;        // PGN
  uint8_t Data_length = 0; // longueur du message (octets)

  uint8_t res_mcp; // résultat de l'état du module MCP2515

  // préparation de l'en-tete du message
  Prio = 2;
  PGN_envoye = 128259;
  RTR = 0;
  Data_length = 8;

  Speed_Water_Referenced_msg = Speed_Water_Referenced*0.5144/0.01;    // calcul de la donné en fonction de ce que la datasheet donne
  Speed_Ground_Referenced_msg = Speed_Ground_Referenced*0.5144/0.01;  // calcul de la donné en fonction de ce que la datasheet donne

  Speed_Direction |= 0xF0;

  Speed_Water_Referenced_msg = ((Speed_Water_Referenced_msg & 0x00FF) << 8) | ((Speed_Water_Referenced_msg & 0xFF0) >> 8);      // Inversion des octet pour respecter l'ordre MSB & LSB
  Speed_Ground_Referenced_msg = ((Speed_Ground_Referenced_msg & 0x00FF) << 8) | ((Speed_Ground_Referenced_msg & 0xFF00) >> 8);  // Inversion des octet pour respecter l'ordre MSB & LSB

  // insertion des bits dans la variable de 64 bits dans l'ordre donné par la datasheet
  message |= ((uint64_t)Sequence_ID) << 56;
  message |= ((uint64_t)Speed_Water_Referenced_msg) << 40;
  message |= ((uint64_t)Speed_Ground_Referenced_msg) << 24;
  message |= ((uint64_t)Speed_Water_Referenced_Type) << 16;
  message |= ((uint64_t)Speed_Direction) << 8;
  message |= 0xFF;

  // remplissage du tableau utilisé pour l'envoie
  for (int i = 0; i < 8; i++)
  {
    mcp_message[i] = (message >> ((7 - i) * 8)) & 0xFF; // décallage vers la droit de x fois (en fonction de la colone du tableau a remplir) puis formatage pour garder juste l'octet voulue
  }

  res_mcp = MCP2515_1.MCP2515_N2K_sendMsg_broadcast(Prio, PGN_envoye, SRC_adresse_envoye, RTR, Data_length, mcp_message);

  return res_mcp;
}

uint8_t NMEA2k_Envoi_PGN_128259(uint8_t SRC_adresse_envoye, uint8_t Sequence_ID, float Speed_Water_Referenced, float Speed_Ground_Referenced, uint8_t Speed_Water_Referenced_Type, uint8_t Speed_Direction)
{
    extern MCP2515_LTG_class MCP2515_1;

    return NMEA2k_Envoi_PGN_128259(SRC_adresse_envoye, Sequence_ID,  Speed_Water_Referenced,  Speed_Ground_Referenced,  Speed_Water_Referenced_Type,  Speed_Direction, MCP2515_1);
}
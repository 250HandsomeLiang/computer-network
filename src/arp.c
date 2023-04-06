#include <string.h>
#include <stdio.h>
#include "net.h"
#include "arp.h"
#include "ethernet.h"
/**
 * @brief 初始的arp包
 * 
 */
static const arp_pkt_t arp_init_pkt = {
    .hw_type16 = constswap16(ARP_HW_ETHER),
    .pro_type16 = constswap16(NET_PROTOCOL_IP),
    .hw_len = NET_MAC_LEN,
    .pro_len = NET_IP_LEN,
    .sender_ip = NET_IF_IP,
    .sender_mac = NET_IF_MAC,
    .target_mac = {0}};

/**
 * @brief arp地址转换表，<ip,mac>的容器
 * 
 */
map_t arp_table;

/**
 * @brief arp buffer，<ip,buf_t>的容器
 * 
 */
map_t arp_buf;

/**
 * @brief 打印一条arp表项
 * 
 * @param ip 表项的ip地址
 * @param mac 表项的mac地址
 * @param timestamp 表项的更新时间
 */
void arp_entry_print(void *ip, void *mac, time_t *timestamp)
{
    printf("%s | %s | %s\n", iptos(ip), mactos(mac), timetos(*timestamp));
}

/**
 * @brief 打印整个arp表
 * 
 */
void arp_print()
{
    printf("===ARP TABLE BEGIN===\n");
    map_foreach(&arp_table, arp_entry_print);
    printf("===ARP TABLE  END ===\n");
}

/**
 * @brief 发送一个arp请求
 * 
 * @param target_ip 想要知道的目标的ip地址
 */
void arp_req(uint8_t *target_ip)
{
    // TO-DO
    //初始化发送缓存
    buf_init(&txbuf, ETHERNET_MAX_TRANSPORT_UNIT + sizeof(ether_hdr_t));
    // 填写ARP报头,即46字节的那个东西
    // 填写目的地的IP
    buf_add_header(&txbuf, 4*sizeof(uint8_t));
    for(int i=0;i<4;i++){
        txbuf.data[i]=target_ip[i];
    }
    // 填写目的MAC地址
    buf_add_header(&txbuf, 6*sizeof(uint8_t));
    for(int i=0;i<6;i++){
        txbuf.data[i]=arp_init_pkt.target_mac[i];
    }
    //填写源IP地址
    buf_add_header(&txbuf, 4*sizeof(uint8_t));
    for(int i=0;i<4;i++){
        txbuf.data[i]=arp_init_pkt.sender_ip[i];
    }
    //填写源MAC地址
    buf_add_header(&txbuf, 6*sizeof(uint8_t));
    for(int i=0;i<6;i++){
        txbuf.data[i]=arp_init_pkt.sender_mac[i];
    }
    //填写操作类型
    buf_add_header(&txbuf, sizeof(uint16_t));
    uint16_t *hdr = (uint16_t *)txbuf.data;
    hdr=swap16(ARP_REQUEST);
    //填写IP地址长度
    buf_add_header(&txbuf, sizeof(uint8_t));
    txbuf.data[0]=arp_init_pkt.pro_len;
    //填写MAC的长度
    buf_add_header(&txbuf, sizeof(uint8_t));
    txbuf.data[0]=arp_init_pkt.hw_len;
    //填写上层协议类型
    buf_add_header(&txbuf, sizeof(uint16_t));
    hdr=(uint16_t *)txbuf.data;
    hdr=arp_init_pkt.pro_type16;
    //填写硬件类型
    buf_add_header(&txbuf, sizeof(uint16_t));
    hdr=(uint16_t *)txbuf.data;
    hdr=arp_init_pkt.hw_type16;
    uint8_t temp_mac[] = {0xff,0xff,0xff,0xff,0xff,0xff};
    ethernet_out(&txbuf,temp_mac,NET_PROTOCOL_ARP);
}

/**
 * @brief 发送一个arp响应
 * 
 * @param target_ip 目标ip地址
 * @param target_mac 目标mac地址
 */
void arp_resp(uint8_t *target_ip, uint8_t *target_mac)
{
    // TO-DO
}

/**
 * @brief 处理一个收到的数据包
 * 
 * @param buf 要处理的数据包
 * @param src_mac 源mac地址
 */
void arp_in(buf_t *buf, uint8_t *src_mac)
{
    // TO-DO
}

/**
 * @brief 处理一个要发送的数据包
 * 
 * @param buf 要处理的数据包
 * @param ip 目标ip地址
 * @param protocol 上层协议
 */
void arp_out(buf_t *buf, uint8_t *ip)
{
    // TO-DO
}

/**
 * @brief 初始化arp协议
 * 
 */
void arp_init()
{
    map_init(&arp_table, NET_IP_LEN, NET_MAC_LEN, 0, ARP_TIMEOUT_SEC, NULL);
    map_init(&arp_buf, NET_IP_LEN, sizeof(buf_t), 0, ARP_MIN_INTERVAL, buf_copy);
    net_add_protocol(NET_PROTOCOL_ARP, arp_in);
    arp_req(net_if_ip);
}
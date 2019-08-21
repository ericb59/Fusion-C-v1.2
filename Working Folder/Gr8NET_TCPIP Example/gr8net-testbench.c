/*
   Testbench for GR8NET internetworking adapter
   MSX TCP-IP UNAPI implementation version 1.0 for Fusion-C / testbench
   Initial release by Eugeny Brychkov 2019-05-31
   License: keeps the same as TCP/IP UNAPI v.1.0 by Konamiman, plus requirement
            to cite original author when this work or its derivatives are used

   Usage: <prog-name> <remote-ip-address>

   remote-ip-address is in decimal dotted notation and must be capable of
   responding to ping requests, accept udp connection to port 53 (DNS)
   and tcp connection to port 80 (HTTP). Obviously, local router/gateway
   would satisfy these requirements.

	To compile testbench (use make-testbench.bat):
	   sdasz80 -o crt0_msxdos.s
	   sdasz80 -o conio.s
	   sdasz80 -o msx-tcpip.s
	   SDCC --code-loc 0x106 --data-loc 0x0 -mz80 --no-std-crt0 --opt-code-size crt0_msxdos.rel msx-tcpip.rel conio.rel msx-tcpip-testbench.c
	   copy /b msx-tcpip-testbench.ihx tcpiptst.ihx
	   hex2bin -e com tcpiptst.ihx
*/

#include <stdio.h>
#include <stdlib.h>
#include"fusion-c/header/gr8net-tcpip.h"
#include "fusion-c/header/msx_fusion.h"


#define	UDP_PORT	53
#define	TCP_PORT	80
#define	ICMP_PROTO	1

void	print_ip(char* ip);
void	put_status(char* text,int status);
void	get_argument(void);
void	cmd_line_error(void);
void	exec_abort(void);
void	break_press(void);
void	break_press_cr(void);
void	print_dump(char* buf, int size);
int	get_tcp_state(int connection);
void	print_udp_state(int status);
void	print_ipraw_state(int status);
void	put_ip_mode(int ipmode);
void	get_ip_info(tcpip_unapi_ip_info* ipinfo);
void	anykey(void);

const	char	ERR_OK_str[]={ "ERR_OK Operation completed successfully" };
const	char	ERR_NOT_IMP_str[]={ "ERR_NOT_IMP Capability not implemented" };
const	char	ERR_NO_NETWORK_str[]={ "ERR_NO_NETWORK No network connection available" };
const	char	ERR_NO_DATA_str[]={ "ERR_NO_DATA No incoming data available" };
const	char	ERR_INV_PARAM_str[]={ "ERR_INV_PARAM Invalid input parameter" };
const	char	ERR_QUERY_EXISTS_str[]={ "ERR_QUERY_EXISTS Another query is already in progress" };
const	char	ERR_INV_IP_str[]={ "ERR_INV_IP Invalid IP address" };
const	char	ERR_NO_DNS_str[]={ "ERR_NO_DNS No DNS servers are configured" };
const	char	ERR_DNS_str[]={ "ERR_DNS Error returned by DNS server" };
const	char	ERR_NO_FREE_CONN_str[]={ "ERR_NO_FREE_CONN No free connections available" };
const	char	ERR_CONN_EXISTS_str[]={ "ERR_CONN_EXISTS Connection already exists" };
const	char	ERR_NO_CONN_str[]={ "ERR_NO_CONN Connection does not exists" };
const	char	ERR_CONN_STATE_str[]={ "ERR_CONN_STATE Invalid connection state" };
const	char	ERR_BUFFER_str[]={ "ERR_BUFFER Insufficient output buffer space" };
const	char	ERR_LARGE_DGRAM_str[]={ "ERR_LARGE_DGRAM Datagram is too large" };
const	char	ERR_INV_OPER_str[]={ "ERR_INV_OPER Invalid operation" };
const	char	ERR_INV_IMPL_str[]={ "ERR_INV_IMPL invalid implementation set by the active_implementation variable" };

const	int	error_number_array[]= { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,255,-1 };
char*	error_str_array[]={ ERR_OK_str,ERR_NOT_IMP_str,ERR_NO_NETWORK_str,ERR_NO_DATA_str,
			ERR_INV_PARAM_str,ERR_QUERY_EXISTS_str,ERR_INV_IP_str,ERR_NO_DNS_str,
			ERR_DNS_str,ERR_NO_FREE_CONN_str,ERR_CONN_EXISTS_str,ERR_NO_CONN_str,
			ERR_CONN_STATE_str,ERR_BUFFER_str,ERR_LARGE_DGRAM_str,ERR_INV_OPER_str,
			ERR_INV_IMPL_str };

const	char	CRLF_str[]={ 13,10,0 };
const	char	hostname[]={ "www.gr8bit.ru" };
const	char	dns_query[]={ 0,1,1,0,0,1,0,0,0,0,0,0,3,'w','w','w',6,'g','r','8','b','i','t',2,'r','u',0,0,1,0,1 };
char	response[1024];
const	char	tcp_data[]={ "GET / HTTP/1.0\r\nUser-Agent: TCP/IP UNAPI test program\r\nAccept: */*;q=0.8\r\nAccept-Language: en-us,en;q=0.5\r\nConnection: close\r\n\r\n" };
const	char	ipraw_ping_request[]={ 8,0,77,85,0,1,0,6,
					97,98,99,100,101,102,103,104,
					105,106,107,108,109,110,111,112,
					113,114,115,116,117,118,119,97,
					98,99,100,101,102,103,104,105 };

char	argument_ip_address[4];
int	implementations;
int	a;
int	i;
int	conn_number;
char*	impl_string;
int	unapi_version;
int	impl_version;
int	net_state;
tcpip_unapi_echo send_echo_param_block;
tcpip_unapi_echo rcv_echo_param_block;
tcpip_unapi_dns_q dns_q;
char	c;
unsigned ttltos_get;
unsigned ttltos_get1;
int	ip_mode;
int	ip_mode1;

int	operations=0;
int	operation_errors=0;

tcpip_unapi_capab_flags_llproto capab_flags_llproto;
tcpip_unapi_capab_connections	capab_connections;
tcpip_unapi_capab_dtg_sizes	capab_dtg_sizes;
tcpip_unapi_ip_info	ip_info;
tcpip_unapi_ip_info	ip_info1;

tcpip_unapi_udp_dtg_parms send_udp_dtg_parms;
tcpip_unapi_udp_state udp_state;
tcpip_unapi_udp_dtg_parms rcv_udp_dtg_parms;
tcpip_unapi_tcp_conn_parms tcp_conn_parms;
tcpip_unapi_tcp_conn_parms state_tcp_conn_parms;

tcpip_unapi_ipraw_parms ipraw_params;
tcpip_unapi_ipraw_state ipraw_state;
tcpip_unapi_ipraw_parms rcv_ipraw_params;

void	main(void)
{
	/* get number of TCPI/IP UNAPI implementations */
	implementations=tcpip_enumerate();
	PrintString("tcpip_enumerate = 0x");
	PrintHex(implementations); PrintString(CRLF_str);
	if(implementations==0)
	{
		PrintString("No implementations found, exiting\r\n");
		return;
	}

	get_argument();
	PrintString("Target IP address: 0x"); print_ip(argument_ip_address);
	break_press_cr();

	/* set currently active implementation */
	active_implementation=0;

	/* get implementation information */
	a=tcpip_impl_getinfo(&impl_string,&unapi_version,&impl_version);
	put_status("tcpip_impl_getinfo",a);
	PrintString(impl_string);
	PrintString("\r\nAPI spec: 0x"); PrintHex(unapi_version);
	PrintString("\r\nAPI impl: 0x"); PrintHex(impl_version);
	break_press_cr();

	/* get capabilities/flags/link level protocol information */
	a=tcpip_get_capab_flags_llproto(&capab_flags_llproto);
	put_status("tcpip_get_capab_flags_llproto",a);
	PrintString("Link level proto: 0x"); PutCharHex(capab_flags_llproto.proto_used);
	PrintString("\r\nFeat. flags: 0x"); PrintHex(capab_flags_llproto.feat_flags);
	PrintString("\r\nCapab. flags: 0x"); PrintHex(capab_flags_llproto.capab_flags);
	break_press_cr();
	anykey();

	/* get number of connections available per type of IP connection */
	a=tcpip_get_capab_connections(&capab_connections);
	put_status("tcpip_get_capab_connections",a);
	PrintString("max_tcp_conn: 0x"); PutCharHex(capab_connections.max_tcp_conn);
	PrintString("\r\nmax_udp_conn: 0x"); PutCharHex(capab_connections.max_udp_conn);
	PrintString("\r\nmax_raw_conn: 0x"); PutCharHex(capab_connections.max_raw_conn);
	PrintString("\r\nfree_tcp_conn: 0x"); PutCharHex(capab_connections.free_tcp_conn);
	PrintString("\r\nfree_udp_conn: 0x"); PutCharHex(capab_connections.free_udp_conn);
	PrintString("\r\nfree_raw_conn: 0x"); PutCharHex(capab_connections.free_raw_conn);
	break_press_cr();

	/* get maximal size of datagrams */
	a=tcpip_get_capab_dtg_sizes(&capab_dtg_sizes);
	put_status("tcpip_get_capab_dtg_sizes",a);
	PrintString("max_outgoing_dtg_size: 0x"); PrintHex(capab_dtg_sizes.max_outgoing_dtg_size);
	PrintString("\r\nmax_incoming_dtg_size: 0x"); PrintHex(capab_dtg_sizes.max_incoming_dtg_size);
	break_press_cr();

	/* get current network state */
	a=tcpip_net_state(&net_state);
	put_status("tcpip_net_state",a);
	PrintString("net_state: 0x"); PutCharHex(net_state);
	break_press_cr();
	anykey();

	/* getting/setting TTL/ToS */
	PrintString("\x0cTest of the TTL/ToS configuration routines\r\n");
	a=tcpip_config_ttltos_get(&ttltos_get);
	put_status("tcpip_config_ttltos_get",a);
	i=0;
	if(a==ERR_OK)
	{
		PrintString("TTL="); PutCharHex(ttltos_get>>8);
		PrintString(", ToS="); PutCharHex(ttltos_get&0xff);
		break_press_cr();

		a=tcpip_config_ttltos_set(ttltos_get-0x101);
		put_status("tcpip_config_ttltos_set",a);
		if(a==ERR_OK)
		{
			a=tcpip_config_ttltos_get(&ttltos_get1);
			put_status("tcpip_config_ttltos_get",a);
			if(a==ERR_OK)
			{
				PrintString("New TTL="); PutCharHex(ttltos_get1>>8);
				PrintString(", ToS="); PutCharHex(ttltos_get1&0xff);
				break_press_cr();
				// revert original values back
				a=tcpip_config_ttltos_set(ttltos_get);
				put_status("tcpip_config_ttltos_set",a);
				i=1;
			}
		}
	}
	PrintString("\n\rTTL/ToS test ");
	if(i==0) PrintString("failed"); else PrintString("successful");
	break_press_cr();
	anykey();

	/* get IP information of the implementation */
	PrintString("\x0cTest of the configuration rotuines\r\n");
	get_ip_info(&ip_info);

	/* auto/fixed configuration test */

__asm
nop
nop
nop
nop
__endasm;


	a=tcpip_config_autoip_get(&ip_mode);
	put_status("tcpip_config_autoip_get",a);
	if(a==ERR_OK)
	{
		put_ip_mode(ip_mode);
		for(i=0;i<4;i++)
		{
			PrintString("\r\nSetting mode 0x"); PrintHex(i);
			a=tcpip_config_autoip_set(i);
			put_status("tcpip_config_autoip_set",a);
			if(a==ERR_OK)
			{
				a=tcpip_config_autoip_get(&ip_mode1);
				put_status("tcpip_config_autoip_get",a);
				if(a==ERR_OK) put_ip_mode(ip_mode1);
			}
			get_ip_info(&ip_info1);
		}
		// revert to original settings
		PrintString("\r\nResetting to original settings...");
		a=tcpip_config_autoip_set(ip_mode);
		put_status("tcpip_config_autoip_set",a);
		a=tcpip_config_autoip_get(&ip_mode1);
		put_status("tcpip_config_autoip_get",a);
		if(a==ERR_OK) put_ip_mode(ip_mode1);
		a=tcpip_config_ip(&ip_info);
		put_status("tcpip_config_autoip_set",a);
		get_ip_info(&ip_info1);
	}
	break_press_cr();
	anykey();

	/* perform PING to the remote device identified in the command line */
	PrintString("\x0cTest of the PING and DNS routines\r\n");
	send_echo_param_block.dest_ip[0]=argument_ip_address[0];
	send_echo_param_block.dest_ip[1]=argument_ip_address[1];
	send_echo_param_block.dest_ip[2]=argument_ip_address[2];
	send_echo_param_block.dest_ip[3]=argument_ip_address[3];
	send_echo_param_block.ttl=128;
	send_echo_param_block.icmp_id=1;
	send_echo_param_block.seq_number=2;
	send_echo_param_block.data_length=64;
	a=tcpip_send_echo(&send_echo_param_block);
	put_status("tcpip_send_echo",a);
	if(a==ERR_OK)
	{
		PrintString("Waiting for incoming ICMP data...");
		for(i=0;i<4000;i++)
		{
			a=tcpip_rcv_echo(&rcv_echo_param_block);
			if(a!=ERR_NO_DATA) break;
		}
		put_status("tcpip_rcv_echo",a);
		if(a!=ERR_NO_DATA)
		{
			if(a==ERR_OK)
			{
				PrintString("response from: 0x"); print_ip(rcv_echo_param_block.dest_ip);
				PrintString("\n\rttl: 0x"); PutCharHex(rcv_echo_param_block.ttl);
				PrintString("\n\ricmp_id: 0x"); PrintHex(rcv_echo_param_block.icmp_id);
				PrintString("\n\rseq_number: 0x"); PrintHex(rcv_echo_param_block.seq_number);
				PrintString("\n\rdata_length: 0x"); PrintHex(rcv_echo_param_block.data_length);
				PrintString(CRLF_str);
			}
		}
	}
	break_press();

	/* perform DNS resolution using DNS_Q/DNS_S rotuines */
	dns_q.flags=0;
	a=tcpip_dns_q(hostname,&dns_q);
	put_status("tcpip_dns_q",a);
	if(a==ERR_OK)
	{
		i=0;
		if((dns_q.state==1) || (dns_q.state==2)) { PrintString("tcpip_dns_q -> "); i=1; }
		else	if(dns_q.state==0)
			{
				for(i=0;i<4000;i++)
				{
					a=tcpip_dns_s(&dns_q);
					if(a!=ERR_OK)
					{
						put_status("tcpip_dns_s",a);
						PrintString("Reason: 0x");
						PutCharHex(dns_q.state);
						PrintString(CRLF_str);
						break;
					}
					if(dns_q.state==2) { PrintString("tcpip_dns_s -> "); i=1; break; }
					else if(dns_q.state==0) { PrintString("tcpip_dns_s: no query!"); break; }
				}
			}
			else	PrintString("tcpip_dns_q returned invalid state");

		if(i!=0)
		{
			PrintString("State/substate/IP: 0x"); PutCharHex(dns_q.state); putchar('/'); PutCharHex(dns_q.substate);
			putchar('/'); print_ip(&dns_q.host_ip[0]);
		}
	}
	break_press();
	anykey();

	/* perform UDP comunication: do DNS query for "www.gr8bit.ru" to the IP address identified in the command line */
	PrintString("\x0cTest of the UDP communication routines\r\n");
	a=tcpip_udp_open(UDP_PORT,CONNTYPE_TRANSIENT,&conn_number);
	put_status("tcpip_udp_open",a);
	if(a==ERR_OK)
	{
		a=tcpip_udp_state(conn_number,&udp_state);
		print_udp_state(a);
		if(a==ERR_OK)
		{
			send_udp_dtg_parms.dest_ip[0]=argument_ip_address[0];
			send_udp_dtg_parms.dest_ip[1]=argument_ip_address[1];
			send_udp_dtg_parms.dest_ip[2]=argument_ip_address[2];
			send_udp_dtg_parms.dest_ip[3]=argument_ip_address[3];
			send_udp_dtg_parms.dest_port=UDP_PORT;
			send_udp_dtg_parms.data_length=sizeof(dns_query);
			a=tcpip_udp_send(conn_number,&send_udp_dtg_parms,dns_query);
			put_status("tcpip_udp_send",a);
			if(a==ERR_OK)
			{
				PrintString("Waiting for incoming UDP data...");
				i=0;
				for(i=0;i<4000;i++)
				{
					a=tcpip_udp_state(conn_number,&udp_state);
					if(a!=ERR_OK) break;
					if(udp_state.num_of_pend_dtg!=0)
					{
						print_udp_state(a);
						a=tcpip_udp_rcv(conn_number,&response[0],1024,&rcv_udp_dtg_parms);
						put_status("tcpip_udp_recv",a);
						if(a==ERR_OK)
						{
							PrintString("response from: "); print_ip(rcv_udp_dtg_parms.dest_ip);
							PrintString("\n\rport: "); PrintHex(rcv_udp_dtg_parms.dest_port);
							PrintString("\n\rdata sizes: "); PrintHex(rcv_udp_dtg_parms.data_length);
							putchar('/'); PrintHex(udp_state.size_of_oldest_dtg);
							print_dump(response,rcv_udp_dtg_parms.data_length);
							i=1;
							break;
						}
					}
				}
				if(i==0) PrintString("UDP: no response received");
			}
		}
		a=tcpip_udp_close(conn_number);
		put_status("tcpip_udp_close",a);
	}
	break_press();
	anykey();

	/* perform TCP comunication: HTTP to the IP address identified in the command line */
	PrintString("\x0cTest of the TCP communication routines\r\n");
	tcp_conn_parms.dest_ip[0]=argument_ip_address[0];
	tcp_conn_parms.dest_ip[1]=argument_ip_address[1];
	tcp_conn_parms.dest_ip[2]=argument_ip_address[2];
	tcp_conn_parms.dest_ip[3]=argument_ip_address[3];
	tcp_conn_parms.dest_port=TCP_PORT;
	tcp_conn_parms.local_port=-1;
	tcp_conn_parms.user_timeout=-1;
	tcp_conn_parms.flags=0;
	a=tcpip_tcp_open(&tcp_conn_parms,&conn_number);
	put_status("tcpip_tcp_open",a);
	if(a==ERR_OK)
	{
		a=get_tcp_state(conn_number);
		if(a==ERR_OK)
		{
			if(state_tcp_conn_parms.send_free_bytes>=sizeof(tcp_data))
			{
				a=tcpip_tcp_send(conn_number,tcp_data,sizeof(tcp_data),0);
				put_status("tcpip_tcp_send",a);
				while((i=get_tcp_state(conn_number))==ERR_OK)
				{
					if((state_tcp_conn_parms.conn_state!=4) && state_tcp_conn_parms.incoming_bytes==0)
					{
						PrintString("\r\nTCP session finished");
						break;
					}
					if(state_tcp_conn_parms.incoming_bytes!=0)
					{
						a=tcpip_tcp_rcv(conn_number,&response[0],1024,&tcp_conn_parms);
						put_status("tcpip_tcp_rcv",a);
						if(a==ERR_OK) print_dump(response,tcp_conn_parms.incoming_bytes);
						else break;
					}
					for(i=0;i<1000;i++);		// delay
				}
			}
			else PrintString("No enough space in TX buffer");
		}
		a=tcpip_tcp_close(conn_number);
		put_status("tcpip_tcp_close",a);
	}
	break_press();
	anykey();

	/* perform RAW comunication: ping the IP address identified in the command line */
	PrintString("\x0cTest of the IPRAW communication routines\r\n");
	a=tcpip_ipraw_open(ICMP_PROTO,CONNTYPE_TRANSIENT,&conn_number);
	put_status("tcpip_ipraw_open",a);
	if(a==ERR_OK)
	{
		a=tcpip_ipraw_state(conn_number,&ipraw_state);
		print_ipraw_state(a);
		if(a==ERR_OK)
		{
			ipraw_params.ip[0]=argument_ip_address[0];
			ipraw_params.ip[1]=argument_ip_address[1];
			ipraw_params.ip[2]=argument_ip_address[2];
			ipraw_params.ip[3]=argument_ip_address[3];
			ipraw_params.data_length=sizeof(ipraw_ping_request);
			a=tcpip_ipraw_send(conn_number,ipraw_ping_request,&ipraw_params);
			put_status("tcpip_ipraw_send",a);
			if(a==ERR_OK)
			{
				PrintString("Waiting for incoming IPRAW/ICMP data...");
				i=0;
				for(i=0;i<4000;i++)
				{
					a=tcpip_ipraw_state(conn_number,&ipraw_state);
					if(a!=ERR_OK) { print_ipraw_state(a); break; }
					if(ipraw_state.num_of_pend_dtg!=0)
					{
						a=tcpip_ipraw_rcv(conn_number,&response[0],1024,&rcv_ipraw_params);
						if((response[0]==0) && (response[4]==0) && (response[5]==1) && (response[6]==0) && (response[7]==6))
						{
							PrintString("response from: "); print_ip(rcv_ipraw_params.ip);
							PrintString("\n\rdata size: "); PrintHex(rcv_ipraw_params.data_length);
							print_dump(response,rcv_ipraw_params.data_length);
							i=1;
							break;
						}
					}
				}
				if(i==0) PrintString("IPRAW: no response received");
			}
		}
		a=tcpip_ipraw_close(conn_number);
	}
	break_press();
	anykey();

	/* this board PING test */
	PrintString("\x0cTest of the PING parameter configuration routines\r\n");
	a=tcpip_config_ping_get(&c);
	put_status("tcpip_config_ping read",a);
	a=tcpip_config_ping_set(1);
	put_status("tcpip_config_ping enabled",a);
	PrintString("Perform ping of this device 0x");  print_ip(ip_info.local_ip);
	PrintString(". Press any key to continue");
	getchar();
	a=tcpip_config_ping_set(0);
	put_status("tcpip_config_ping disabled",a);
	PrintString("Perform ping of this device 0x");  print_ip(ip_info.local_ip);
	PrintString(". Press any key to continue");
	getchar();
	a=tcpip_config_ping_set(c);
	put_status("tcpip_config_ping restored",a);

	/* finalize workflow */
	PrintString("\r\nOperations 0x"); PrintHex(operations);
	PrintString(", errors 0x"); PrintHex(operation_errors);
}

void	put_ip_mode(int ipmode)
{
	PrintString("autoip: ip="); if(ipmode&1) PrintString("yes"); else PrintString("no");
	PrintString(", dns="); if(ipmode&2) PrintString("yes"); else PrintString("no");
	break_press_cr();
}

void	get_ip_info(tcpip_unapi_ip_info* ipinfo)
{
	a=tcpip_get_ipinfo(ipinfo);
	put_status("tcpip_get_ipinfo",a);
	if(a==ERR_OK)
	{
		PrintString("local_ip: 0x"); print_ip(ipinfo->local_ip);
		PrintString("\r\npeer_ip: 0x"); print_ip(ipinfo->peer_ip);
		PrintString("\r\nsubnet_mask: 0x"); print_ip(ipinfo->subnet_mask);
		PrintString("\r\ngateway_ip: 0x"); print_ip(ipinfo->gateway_ip);
		PrintString("\r\ndns_ip_pri: 0x"); print_ip(ipinfo->dns_ip_pri);
		PrintString("\r\ndns_ip_sec: 0x"); print_ip(ipinfo->dns_ip_sec);
	}
	break_press_cr();
}

void	print_ipraw_state(int status)
{
	put_status("tcpip_ipraw_state",status);
	if(status==ERR_OK)
	{
		PrintString("Proto: 0x"); PutCharHex(ipraw_state.proto_code);
		PrintString(CRLF_str); PrintString("Sizes: 0x"); PrintHex(ipraw_state.num_of_pend_dtg);
		putchar('/'); PrintHex(ipraw_state.size_of_oldest_dtg);
		PrintString(CRLF_str);
	}
}

int	get_tcp_state(int connection)
{
int	ret_status;
	ret_status=tcpip_tcp_state(connection,&state_tcp_conn_parms);
	put_status("tcpip_tcp_state",a);
	if(ret_status==ERR_OK)
	{
		PrintString("IP/ports: 0x"); print_ip(state_tcp_conn_parms.dest_ip);
		putchar('/'); PrintHex(state_tcp_conn_parms.dest_port);
		putchar('/'); PrintHex(state_tcp_conn_parms.local_port);
		PrintString("\r\nState: 0x"); PutCharHex(state_tcp_conn_parms.conn_state);
		putchar('/'); PutCharHex(state_tcp_conn_parms.close_reason);
		PrintString("\r\nSizes: 0x"); PrintHex(state_tcp_conn_parms.incoming_bytes);
		putchar('/'); PrintHex(state_tcp_conn_parms.urgent_incoming_bytes);
		putchar('/'); PrintHex(state_tcp_conn_parms.send_free_bytes);
		PrintString(CRLF_str);
	}
	return(ret_status);
}

void	print_udp_state(int status)
{
	put_status("tcpip_udp_state",a);
	if(status==ERR_OK)
	{
		PrintString("Dtg/size/port: 0x"); PrintHex(udp_state.num_of_pend_dtg);
		putchar('/'); PrintHex(udp_state.size_of_oldest_dtg);
		putchar('/'); PrintHex(udp_state.port_number);
	}
}

int	w;
int	g,g1,g2;
char	chr;

void	print_dump(char* buf, int size)
{
	w=(int)(((*(char*)0xf3b0)-1)>>2);
	g1=size;
	while(g1>0)
	{
		if(g1<w) g2=g1; else g2=w;
		PrintString(CRLF_str);
		for(g=0;g<g2;g++)
		{
			PutCharHex(*(buf+g));
			putchar(' ');
		}
		for(g=0;g<g2;g++)
		{
			chr=*(buf+g);
			if((chr<0x20) || (chr==0x7f) || (chr==0xff)) chr='.';
			putchar(chr);
		}
		buf+=g2;
		g1-=g2;
	}
}

void	print_ip(char* ip)
{
	PutCharHex(ip[0]);
	putchar('.');
	PutCharHex(ip[1]);
	putchar('.');
	PutCharHex(ip[2]);
	putchar('.');
	PutCharHex(ip[3]);
}

void	put_status(char* text,int status)
{
int	c,d;
	PrintString(CRLF_str);
	PrintString(text);
	PrintString(" = 0x");
	PrintHex(status);
	operations++;
	if(status!=0) operation_errors++;
	c=0;
	do
	{
		d=error_number_array[c];
		if(d==status)
		{
			PrintString(" (");
			PrintString(error_str_array[c]);
			PrintString(")\r\n");
			return;
		}
		c++;
	}
	while(d!=-1);
	PrintString(" (Invalid error code)\r\n");
}

// Get IP address out of the argument string
void	get_argument()
{
char*	ptr;		// pointer to the command line argument area
char*	ip_addr;	// resulting IP address pointer
char	cc;		// current byte
int	octet;		// current octet
int	octet_number;	// current octet number

	ptr=(char*)0x81;			// start of the MSX-DOS argument string
	ip_addr=&argument_ip_address[0];	// ip address storage space start
	while(*ptr==0x20) ptr++;		// skip leading space characters

	for(octet_number=0;octet_number<4;octet_number++)
	{
		cc=*ptr;				// get first char
		if(cc<0x30 || cc>0x39) cmd_line_error();// error if not digit
		octet=0;				// initialize octet value
		do
		{
			octet*=10;
			octet+=cc-0x30;
			ptr++;
			cc=*ptr;			// get next char out of arg string
		}
		while(cc>=0x30 && cc<=0x39);		// loop until not digit char found
		if(cc!='.' && cc!=0xd && cc!=0) cmd_line_error();	// error if unexpected char
		if(cc==0xd && cc==0) break;		// end of arg string -> finished
		*ip_addr=octet;
		ip_addr++;
		ptr++;					// skip control character
	}
}

void	break_press_cr(void)
{
	PrintString(CRLF_str);
	if(CheckBreak()!=0) exec_abort();
}

void	anykey(void)
{
char	c;
	PrintString("\r\nPress any key for the next test");
	c=getchar();
	PrintString(CRLF_str);
	if(c==3) exec_abort();
}

void	break_press(void)
{
	if(CheckBreak()!=0) exec_abort();
}

void	cmd_line_error(void)
{
	PrintString("Error in command line"); exec_abort();
}

void	exec_abort(void)
{
	__asm
	jp	0x0
	__endasm;
}

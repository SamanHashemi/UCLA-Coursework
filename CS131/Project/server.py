import sys, time, re
import asyncio
import aiohttp
import async_timeout
import json

clients = {}
API_KEY = "AIzaSyB1u8IgKSHb8vYsVNL3Oz1tpV2pHPC7uMc"
API_ENDPOINT = "https://maps.googleapis.com/maps/api/place/nearbysearch/json"
HOST = "localhost"
SERVER_ID = 0
TIME_DIFF = 1
LATLONG = 2
TIME_SENT = 3
tasks = {}

ports = {'Goloman': 12310, 'Hands': 12311, 'Holiday': 12312, 'Wilkes': 12313, 'Welsh': 12314}

servercomm = {
    'Goloman': ['Hands', 'Holiday', 'Wilkes'],
    'Hands': ['Goloman', 'Wilkes'],
    'Holiday': ['Goloman', 'Welsh'],
    'Wilkes': ['Goloman', 'Hands'],
    'Welsh': ['Holiday'],
}


def main():
    global servID, l, f, event_loop
    if len(sys.argv) != 2:
        print('ERROR: ARG NUMBER INVALID, SPECIFY PORT NUMBER')
        exit(1)
    servID = sys.argv[1]
    if servID not in [*ports]:
        print('ERROR: PORT ID INVALID')
        exit(1)

    l = '%s.txt' % servID
    print(servID)

    f = open(l, 'a+')

    event_loop = asyncio.get_event_loop()

    server = event_loop.run_until_complete(asyncio.start_server(client_conn, HOST, ports[servID], loop=event_loop))

    print('SERVER {}'.format(server.sockets[0].getsockname()))
    try:
        event_loop.run_forever()
    except KeyboardInterrupt:
        f.close()
        print('\nCLOSING...')

    server.close()
    event_loop.run_until_complete(server.wait_closed())
    event_loop.close()

    f.close()

async def serv_write(w, txt):
    if txt is not None:
        await stream_write(w, txt)
        w.close()
    else:
        return


async def stream_write(w, txt):
    if txt is not None:
        try:
            w.write(txt.encode())
            await w.drain()
        except:
            print('IOError in stream_write: %s' % txt)
    else:
        return


async def log_write(txt):
    if txt is not None:
        try:
            f.write(txt)
        except:
            print('IOError in log_write: %s' % txt)
    else:
        return


async def tcp_conn(txt, ds):
    for serv in servercomm[servID]:
        if serv not in ds:
            try:
                r, w = await asyncio.open_connection(HOST, ports[serv], loop=event_loop)
                await serv_write(w, txt)
                await log_write('CONNECTED WITH %s\n'
                                   'MESSAGE: %s\n '
                                   'DISCONNECTED WITH %s\n' % (serv, txt, serv))
            except:
                print('ERROR W/ CONNECTIONG AND MESSAGE PROP TO %s' % serv)
                await log_write(
                    'ERROR W/ CONNECTIONG AND MESSAGE PROP TO %s: DROPPED CONNECTION TO %s\n' %
                    (serv, serv))
        else:
            continue


def client_conn(r, w):
    t = asyncio.ensure_future(client_handler(r, w))
    tasks[t] = (r, w)

    def client_close(t):
        del tasks[t]
        w.close()
        print("CLOSED CLIENT")

    t.add_done_callback(client_close)


space_remover = re.compile(r'\s+')
async def client_handler(r, w):
    if r.at_eof():
        return
    while True:
        bufr = list(filter(lambda x: len(x) > 0, space_remover.sub(r' ',
                                                                   (await r.readline()).decode()).strip().split(' ')))
        await buf_check(w, bufr)
        print(bufr)
        if r.at_eof():
            break


async def buf_check(w, bufr):
    received_time = time.time()

    print('HANDLING ' + str(bufr))

    if len(bufr) < 4:
        return

    cmd = bufr[0]
    if cmd not in ['IAMAT', 'WHATSAT', 'AT']:
        res = '? %s' % ' '.join(bufr)
        input_cmd = '%s' % ' '.join(bufr)
    else:
        if not (await cmd_check(cmd, bufr[1:])):
            res = '? %s' % ' '.join(bufr)
            input_cmd = '%s' % ' '.join(bufr)
        else:
            res = await command_handler(cmd, bufr[1:], received_time)
            input_cmd = '%s %s' % (cmd, ' '.join(bufr[1:]))

    await stream_write(w, res)
    await log_write('RECIEVED INPUT: ' + str(input_cmd) + '\n')
    await log_write('SEND OUTPUT: ' + str(res) + '\n')


async def command_handler(cmd, msg_arr, tr):
    res = None
    if cmd == 'IAMAT':
        timeoff = tr - float(msg_arr[2])
        if timeoff > 0:
            timeoff = '+%f' % timeoff
        else:
            timeoff = '-%f' % timeoff

        clients[msg_arr[0]] = [servID, timeoff, msg_arr[1], msg_arr[2]]

        res = str('AT %s %s %s %s %s\n') % (servID, timeoff, msg_arr[0], msg_arr[1], msg_arr[2])
        prop_msg = 'AT %s %s %s %s %s %s\n' % (servID, timeoff, msg_arr[0], msg_arr[1], msg_arr[2], servID)

        connect = asyncio.ensure_future(tcp_conn(prop_msg, []))

        def conn_complete(task):
            print('MESSAGES TO SERVERS {}'.format(servercomm[servID]))

        connect.add_done_callback(conn_complete)

    elif cmd == 'WHATSAT':
        if msg_arr[0] in clients:
            tmp_srv, timeoff, lat_long, send_time = clients[msg_arr[0]]

            lat_arr = list(filter(lambda x: len(x) > 0, re.split(r'[+-]', lat_long)))

            url = 'https://maps.googleapis.com/maps/api/place/nearbysearch/json?location=%s,%s&radius=%d&key=%s' % (
                lat_arr[0], lat_arr[1], int(msg_arr[1]), API_KEY)

            async with aiohttp.ClientSession() as session:
                (await get_sess(session, url))['results'] = (await get_sess(session, url))['results'][:int(msg_arr[2])]
                api_resp = json.dumps((await get_sess(session, url)), indent=3)

            res = 'AT %s %s %s %s %s\n%s\n\n' % (tmp_srv, timeoff, msg_arr[0], lat_long, send_time, api_resp)

    elif cmd == 'AT':
        print('CONNECTED WITH %s\n' % msg_arr[5])
        print('DISCONNECTED FROM %s AFTER MESSAGE ->\n' % msg_arr[5])
        await log_write('CONNECTED WITH %s\n' % msg_arr[5])
        await log_write('DISCONNECTED FROM %s AFTER MESSAGE ->\n' % msg_arr[5])

        if msg_arr[2] not in clients or float(msg_arr[4]) > float(clients[msg_arr[2]][TIME_SENT]):

            clients[msg_arr[2]] = [msg_arr[0], msg_arr[1], msg_arr[3], msg_arr[4]]

            prop_msg = '%s %s %s\n' % (cmd, ' '.join(msg_arr[:-1]), servID)

            connect = asyncio.ensure_future(tcp_conn(prop_msg, [msg_arr[5], msg_arr[0]]))

            def conn_complete(task):
                print('MESSAGES TO SERVER {}'.format(servercomm[servID]))

            connect.add_done_callback(conn_complete)
        else:
            return None

    return res


async def get_sess(ses, addr):
    async with async_timeout.timeout(10):
        async with ses.get(addr) as response:
            return await response.json()


async def cmd_check(cmd, r):
    valid_field = re.compile(r'^\S+$')
    if not all([valid_field.match(x) for x in r]):
        return False
    latlong_check = re.compile(r'^[+-][0-9]+.[0-9]+[+-][0-9]+.[0-9]+$')
    time_check = re.compile(r'^[0-9]*.[0-9]+$|^[0-9]+.[0-9]*$')
    if cmd == 'IAMAT':
        if len(r) != 3:
            return False
        if not (latlong_check.match(r[1]) and time_check.match(r[2])):
            return False

    elif cmd == 'WHATSAT':
        if len(r) != 3:
            return False
        int_check = re.compile(r'^[0-9]+$')
        if not (int_check.match(r[1]) and int_check.match(r[2])):
            return False

        if (int(r[1]) > 50 or int(r[1]) < 0) or (r[0] not in clients) or (
                int(r[2]) > 20 or int(r[2]) < 0):
            return False

        if r[0] not in clients:
            return False

    elif cmd == 'AT':
        if len(r) != 6:
            return False
        time_diff_re = re.compile(r'^[+-][0-9]+.[0-9]+$')
        if not (time_diff_re.match(r[1]) and latlong_check.match(r[3]) and time_check.match(r[4])):
            return False
    else:
        return False

    return True

if __name__ == '__main__':
    main()

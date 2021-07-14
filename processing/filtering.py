import pandas as pd
from influxdb import InfluxDBClient

client = InfluxDBClient(host='floodnet-influxdb.sonycproject.com', ssl=True, port=0, database='db0')

measure = 'distance'
table = 'msg'
app_id = 'deployment_one_app'
dev_id = 'sensor_2'
days_back = 7

query = 'SELECT "%s" ' \
        'FROM "%s" ' \
        'WHERE "app_id" = \'%s\' ' \
        'AND "dev_id" = \'%s\' ' \
        'AND "time" >= now() - %id'\
        % (measure, table, app_id, dev_id, days_back)

data = client.query(query)

value_data = data.raw['series'][0]['values']

df = pd.DataFrame(value_data, columns =['datetime', 'distance'])

df['datetime'] = pd.to_datetime(df['datetime'], format='%Y-%m-%dT%H:%M:%S.%f')
df['localtime'] = df['datetime'].dt.tz_convert('US/Eastern')

print(df)
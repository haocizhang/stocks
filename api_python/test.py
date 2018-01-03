from InvestopediaApi import ita
import json

client = ita.Account("zhanghaoci@gmail.com", "haocizhang")

status = client.get_portfolio_status()
data = {}
data["cash"] = status.cash
data["buying_power"] = status.buying_power
data["account_val"] = status.account_val
data["annual_return"] = status.annual_return
json_data = json.dumps(data)

print(json_data)

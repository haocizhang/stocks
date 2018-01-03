from InvestopediaApi import ita
import json
import sys

company_name = sys.argv[1]
volume = sys.argv[2]
client = ita.Account("zhanghaoci@gmail.com", "haocizhang")
client.trade(company_name ,ita.Action.buy, volume)

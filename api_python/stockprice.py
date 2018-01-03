from InvestopediaApi import ita
import sys
import mechanicalsoup

def getprice(c):
	BASE_URL = 'http://www.investopedia.com'
	br = mechanicalsoup.Browser()
	response = br.get(BASE_URL + '/markets/stocks/' + c.lower())
	quote_id = "quotePrice"
	parsed_html = response.soup
	try:
		quote = parsed_html.find('td', attrs={'id': quote_id}).text
	except:
		return False
	return quote.replace(",","")


company_name = sys.argv[1]
print(getprice(company_name))
